#include <xaudio2.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>

#pragma comment(lib, "xaudio2.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "ogg.lib")
#pragma comment(lib, "vorbis.lib")
#pragma comment(lib, "vorbisfile.lib")

namespace Audio
{
#define MAXCHANNELS 32
#define max_audio 256

	struct RIFF_HEADER {
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
	};

	struct WAVE_FORMAT {
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long byteRate;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
	};

	struct WAVE_DATA {
		char subChunkId[4];
		unsigned long subChunkSize;
	};

	struct soundDesc {
		std::vector<BYTE> data;
		WAVEFORMATEX format;
	};

	struct VoiceController {
		IXAudio2SourceVoice* voice;
		int soundIndex;
		bool isLooping;
		float volume;

		VoiceController() : voice(nullptr), soundIndex(-1), isLooping(false), volume(1.0f) {}
		VoiceController(IXAudio2SourceVoice* v, int idx, bool loop, float vol)
			: voice(v), soundIndex(idx), isLooping(loop), volume(vol) {
		}
	};

	struct OggMemoryFile {
		const BYTE* data;
		size_t size;
		size_t pos;
	};

	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
	XAUDIO2_BUFFER buffer;
	BYTE* channel[MAXCHANNELS];

	int len;
	int channelLen;

	soundDesc Sounds[max_audio];
	std::unordered_map<std::string, int> SoundName;

	std::list<VoiceController> activeVoices;

	int soundsCount;

	// Колбэки чтения для ov_open_callbacks
	static size_t OggMemoryRead(void* ptr, size_t size, size_t nmemb, void* datasource) {
		OggMemoryFile* mem = (OggMemoryFile*)datasource;
		size_t bytesToRead = size * nmemb;
		if (mem->pos + bytesToRead > mem->size) {
			bytesToRead = mem->size - mem->pos;
		}
		memcpy(ptr, mem->data + mem->pos, bytesToRead);
		mem->pos += bytesToRead;
		return bytesToRead / size;
	}

	static int OggMemorySeek(void* datasource, ogg_int64_t offset, int whence) {
		OggMemoryFile* mem = (OggMemoryFile*)datasource;
		switch (whence) {
		case SEEK_SET: mem->pos = (size_t)offset; break;
		case SEEK_CUR: mem->pos = (size_t)(mem->pos + offset); break;
		case SEEK_END: mem->pos = (size_t)(mem->size + offset); break;
		}
		if (mem->pos > mem->size) mem->pos = mem->size;
		return 0;
	}

	static long OggMemoryTell(void* datasource) {
		OggMemoryFile* mem = (OggMemoryFile*)datasource;
		return (long)mem->pos;
	}

	// Статический набор колбэков
	static ov_callbacks OV_CALLBACKS_MEMORY = {
		OggMemoryRead,
		OggMemorySeek,
		nullptr, // close (нам не нужно)
		OggMemoryTell
	};

	void Init() {
		IXAudio2SourceVoice* pSourceVoice;

		HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (FAILED(hr)) {
			Log("Failed to coinitialize ex");
			return;
		}

		hr = XAudio2Create(&pXAudio2, 0);
		if (FAILED(hr)) {
			Log("Failed to create XAudio2");
			return;
		}

		hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice);
		if (FAILED(hr)) {
			Log("Failed to create mastering voice");
			return;
		}

		for (int x = 0; x < MAXCHANNELS - 1; x++)
		{
			channel[x] = new BYTE[channelLen * 2 * 2];
			ZeroMemory(channel[x], channelLen * 2 * 2);
		}

		channel[MAXCHANNELS - 1] = new BYTE[len * 2 * 2];
		ZeroMemory(channel[MAXCHANNELS - 1], len * 2 * 2);

		WAVEFORMATEX waveformat;
		waveformat.wFormatTag = WAVE_FORMAT_PCM;
		waveformat.nChannels = 2;
		waveformat.nSamplesPerSec = 44100;
		waveformat.nAvgBytesPerSec = 44100 * 2 * 2;
		waveformat.nBlockAlign = 2 * 16 / 8;
		waveformat.wBitsPerSample = 16;
		waveformat.cbSize = 0;

		hr = pXAudio2->CreateSourceVoice(&pSourceVoice, &waveformat);

		ZeroMemory(&buffer, sizeof(buffer));
		buffer.AudioBytes = 2 * 2 * len;
		buffer.pAudioData = channel[MAXCHANNELS - 1];
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		buffer.PlayBegin = 0;
		buffer.PlayLength = 0;

		hr = pSourceVoice->SubmitSourceBuffer(&buffer);
		pSourceVoice->Stop(0, 0);
	};

	void Release() {
		// Сначала уничтожаем все активные голоса
		for (auto& voiceController : activeVoices) {
			if (voiceController.voice) {
				voiceController.voice->DestroyVoice();
			}
		}
		activeVoices.clear();

		// Затем освобождаем буферы каналов
		for (int x = 0; x < MAXCHANNELS; x++) {
			if (channel[x]) {
				delete[] channel[x];
				channel[x] = nullptr;
			}
		}

		if (pMasteringVoice) {
			pMasteringVoice->DestroyVoice();
		}
		if (pXAudio2) {
			pXAudio2->StopEngine();
			pXAudio2->Release();
		}
	};

	IXAudio2SourceVoice* Play(int soundIndex, bool loop = false, float volume = 1.0f) {
		if (soundIndex < 0 || soundIndex >= soundsCount) return nullptr;

		soundDesc& sound = Sounds[soundIndex];
		if (sound.data.empty()) return nullptr;

		IXAudio2SourceVoice* pVoice = nullptr;
		HRESULT hr = pXAudio2->CreateSourceVoice(&pVoice, &sound.format);
		if (FAILED(hr)) return nullptr;

		XAUDIO2_BUFFER voiceBuffer = {};
		voiceBuffer.pAudioData = sound.data.data();
		voiceBuffer.AudioBytes = (UINT32)sound.data.size();
		voiceBuffer.Flags = XAUDIO2_END_OF_STREAM;

		// Если зациклено, устанавливаем флаг
		if (loop) {
			voiceBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
			voiceBuffer.LoopBegin = 0;
			voiceBuffer.LoopLength = 0;
		}

		hr = pVoice->SubmitSourceBuffer(&voiceBuffer);
		if (FAILED(hr)) {
			pVoice->DestroyVoice();
			return nullptr;
		}

		// Устанавливаем громкость
		if (volume < 0.0f) volume = 0.0f;
		if (volume > 1.0f) volume = 1.0f;
		pVoice->SetVolume(volume);

		pVoice->Start(0);

		// Сохраняем контроллер
		activeVoices.push_back(VoiceController(pVoice, soundIndex, loop, volume));

		return pVoice;
	}

	IXAudio2SourceVoice* Play(const std::string& name, bool loop = false, float volume = 1.0f) {
		auto it = SoundName.find(name);
		if (it != SoundName.end()) {
			return Play(it->second, loop, volume);
		}
		return nullptr;
	}

	void Pause(IXAudio2SourceVoice* pVoice) {
		if (pVoice) {
			pVoice->Stop(0);
		}
	}

	void Resume(IXAudio2SourceVoice* pVoice) {
		if (pVoice) {
			pVoice->Start(0);
		}
	}

	void Stop(IXAudio2SourceVoice* pVoice) {
		if (pVoice) {
			pVoice->Stop(0);
		}
	}

	void Stop(const std::string& name) {
		auto it = SoundName.find(name);
		if (it == SoundName.end()) return;

		int soundIndex = it->second;
		for (auto& controller : activeVoices) {
			if (controller.voice) {
				controller.voice->Stop(0);
			}
		}
	}

	void StopAll() {
		for (auto& controller : activeVoices) {
			if (controller.voice) {
				controller.voice->Stop(0);
				controller.voice->DestroyVoice();
				controller.voice = nullptr;
			}
		}
		activeVoices.clear();
	}

	void SetVolume(IXAudio2SourceVoice* pVoice, float volume) {
		if (pVoice) {
			if (volume < 0.0f) volume = 0.0f;
			if (volume > 1.0f) volume = 1.0f;

			pVoice->SetVolume(volume);
		}
	}

	void SetVolume(const std::string& name, float volume) {
		auto it = SoundName.find(name);
		if (it == SoundName.end()) return;

		int soundIndex = it->second;
		for (auto& controller : activeVoices) {
			if (controller.voice) {
				SetVolume(controller.voice, volume);
			}
		}
	}

	float GetVolume(IXAudio2SourceVoice* pVoice) {
		if (!pVoice) return 0.0f;
		float volume;
		pVoice->GetVolume(&volume);
		return volume;
	}

	void DeleteVoice(IXAudio2SourceVoice* pVoice) {
		pVoice->DestroyVoice();
	};

	bool IsPlaying(IXAudio2SourceVoice* pVoice) {
		if (!pVoice) return false;
		XAUDIO2_VOICE_STATE state;
		pVoice->GetState(&state);
		return state.BuffersQueued > 0;
	}

	void UpdateVoices() {
		for (auto it = activeVoices.begin(); it != activeVoices.end(); ) {
			if (!it->voice || !IsPlaying(it->voice)) {
				if (it->voice) {
					it->voice->DestroyVoice();
				}
				it = activeVoices.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void LoadWavFile(const std::string name, const char* filename) {
		if (soundsCount >= max_audio) {
			Log("Cannot load sound: limit (");
			Log(std::to_string(max_audio).c_str());
			Log(") has reached\n");
			return;
		}

		int curSnd = soundsCount++;
		SoundName[name] = curSnd;

		soundDesc& sound = Sounds[curSnd];

		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			soundsCount--; // откатываем счётчик
			return;
		}

		RIFF_HEADER riffHeader;
		WAVE_FORMAT waveFormatHeader;
		WAVE_DATA waveDataHeader;

		file.read((char*)&riffHeader, sizeof(RIFF_HEADER));
		if (riffHeader.chunkId[0] != 'R' || riffHeader.chunkId[1] != 'I' ||
			riffHeader.chunkId[2] != 'F' || riffHeader.chunkId[3] != 'F' ||
			riffHeader.format[0] != 'W' || riffHeader.format[1] != 'A' ||
			riffHeader.format[2] != 'V' || riffHeader.format[3] != 'E') {
			soundsCount--;
			return;
		}

		bool fmtFound = false;
		while (!fmtFound && !file.eof()) {
			file.read((char*)&waveFormatHeader, sizeof(WAVE_FORMAT));
			if (waveFormatHeader.subChunkId[0] == 'f' && waveFormatHeader.subChunkId[1] == 'm' &&
				waveFormatHeader.subChunkId[2] == 't' && waveFormatHeader.subChunkId[3] == ' ') {
				fmtFound = true;
				if (waveFormatHeader.subChunkSize > sizeof(WAVE_FORMAT) - 8) {
					file.seekg(waveFormatHeader.subChunkSize - (sizeof(WAVE_FORMAT) - 8), std::ios::cur);
				}
			}
			else {
				file.seekg(waveFormatHeader.subChunkSize, std::ios::cur);
			}
		}

		if (!fmtFound) { soundsCount--; return; }

		bool dataFound = false;
		while (!dataFound && !file.eof()) {
			file.read((char*)&waveDataHeader, sizeof(WAVE_DATA));
			if (waveDataHeader.subChunkId[0] == 'd' && waveDataHeader.subChunkId[1] == 'a' &&
				waveDataHeader.subChunkId[2] == 't' && waveDataHeader.subChunkId[3] == 'a') {
				dataFound = true;
			}
			else {
				file.seekg(waveDataHeader.subChunkSize, std::ios::cur);
			}
		}

		if (!dataFound) { soundsCount--; return; }

		// Заполняем формат в структуре sound
		sound.format.wFormatTag = WAVE_FORMAT_PCM;
		sound.format.nChannels = waveFormatHeader.numChannels;
		sound.format.nSamplesPerSec = waveFormatHeader.sampleRate;
		sound.format.wBitsPerSample = waveFormatHeader.bitsPerSample;
		sound.format.nBlockAlign = waveFormatHeader.blockAlign;
		sound.format.nAvgBytesPerSec = waveFormatHeader.byteRate;
		sound.format.cbSize = 0;

		// Читаем данные прямо в sound.data
		sound.data.resize(waveDataHeader.subChunkSize);
		file.read((char*)sound.data.data(), waveDataHeader.subChunkSize);

		file.close();

		Log("Wav file " + name + " loaded successfully\n");
	};

	void LoadOggFile(const std::string& name, const char* filename) {
		if (soundsCount >= max_audio) {
			Log("Cannot load sound: limit (");
			Log(std::to_string(max_audio).c_str());
			Log(") has reached\n");
			return;
		}

		// Читаем весь файл в память
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			Log("Cannot open OGG file\n");
			return;
		}

		std::streamsize fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::vector<BYTE> fileData(fileSize);
		file.read((char*)fileData.data(), fileSize);
		file.close();

		// Настройка для чтения из памяти
		OggMemoryFile memFile;
		memFile.data = fileData.data();
		memFile.size = fileSize;
		memFile.pos = 0;

		OggVorbis_File vf;
		int result = ov_open_callbacks(&memFile, &vf, nullptr, 0, OV_CALLBACKS_MEMORY);
		if (result != 0) {
			Log("Not a valid Ogg Vorbis file\n");
			return;
		}

		// Получаем информацию о потоке
		vorbis_info* vi = ov_info(&vf, -1);
		if (!vi) {
			ov_clear(&vf);
			return;
		}

		int numChannels = vi->channels;
		int sampleRate = (int)vi->rate;
		int bitsPerSample = 16; // Будем декодировать в 16 бит

		// Выделяем буфер для PCM данных (максимум 1 секунда за раз)
		const size_t bufferSize = sampleRate * numChannels * (bitsPerSample / 8); // 1 сек
		std::vector<short> pcmBuffer(bufferSize);

		std::vector<BYTE> pcmData; // Сюда соберём весь PCM
		int bitstream = 0;
		long bytesRead = 0;

		// Декодируем весь файл
		while (true) {
			bytesRead = ov_read(&vf, (char*)pcmBuffer.data(),
				(int)(bufferSize * sizeof(short)), 0, 2, 1, &bitstream);
			if (bytesRead <= 0) break; // EOF или ошибка

			size_t oldSize = pcmData.size();
			pcmData.resize(oldSize + bytesRead);
			memcpy(pcmData.data() + oldSize, pcmBuffer.data(), bytesRead);
		}

		ov_clear(&vf);

		if (pcmData.empty()) {
			Log("No audio data decoded\n");
			return;
		}

		// Сохраняем в наш массив звуков
		int curSnd = soundsCount++;
		SoundName[name] = curSnd;

		soundDesc& sound = Sounds[curSnd];
		sound.data = std::move(pcmData);

		// Заполняем WAVEFORMATEX
		sound.format.wFormatTag = WAVE_FORMAT_PCM;
		sound.format.nChannels = numChannels;
		sound.format.nSamplesPerSec = sampleRate;
		sound.format.wBitsPerSample = bitsPerSample;
		sound.format.nBlockAlign = numChannels * bitsPerSample / 8;
		sound.format.nAvgBytesPerSec = sampleRate * sound.format.nBlockAlign;
		sound.format.cbSize = 0;

		Log("Ogg file " + name + " loaded successfully\n");
	};
}