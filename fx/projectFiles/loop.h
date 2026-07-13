

#include "gApi.h"
#include "camera.h"
#include "cubemap.h"
#include "object.h"

XMVECTOR F2V(float4 v)
{
	return XMVECTOR{ v.x,v.y,v.z,v.w };
}

float4 V2F(XMVECTOR v)
{
	return float4{ XMVectorGetX(v),XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetW(v)};
}

XMVECTOR getRandVector4()
{
	return XMVECTOR{
		Object::getRandFloat(),
		Object::getRandFloat(),
		Object::getRandFloat(),
		Object::getRandFloat()
	};
}

XMVECTOR tUP = { 0,1,0 };
float4 follow = { 0,0,11,0 };
float4 curent_i = { 0,0,11,0 };
XMVECTOR p0 = { 0,0,0 };
XMVECTOR p1 = { 0,0,1 };

struct hero_ {

	XMVECTOR pos = { 0,0,0,0 };
	XMVECTOR forward = { 0,0,0 };
	XMVECTOR upBeforeJump = { 0,0,0 };
	XMVECTOR forwardBeforeJump = { 0,0,0 };
	int lineIndex = 0;
	float pointIndex = 0;

	float speedFactor = 0;
	float speed = 0;
	float accel = 0.01;
	float maxSpeed = .8;
	float autoBrake = .9;

	float axisAngle = 0;
	float axisAngleSpeed = 0;
	float axisAngleAccel = 0.01;
	float maxAxisAngleSpeed = .1;

	float cameraAngle = 100;

	bool jump = false;
	float jumpHeight = 0;

	float yOffset = 40;

	struct {
		bool mode = true;
		float progress = 0.0f;
		float acceleratedT = 0;
	} gravity;


	void ProcessGravity()
	{
		float minDistance = 1e9f; // Инициализируем заведомо большим числом
		float4 closestPoint = { 0.0f, 0.0f, 0.0f, 0.0f };
		bool foundPoint = false;

		//Находим ОДНУ самую ближайшую точку
		for (int i = 0; i < Object::starLineList.lineCount; i++)
		{
			for (int j = 0; j < Object::starLineList.line[i].pointCount; j++)
			{

				float dst = distance(V2F(pos), Object::starLineList.line[i].point[j]);

				// Если эта точка ближе, чем все предыдущие, запоминаем её
				if (dst < minDistance)
				{
					minDistance = dst;
					closestPoint = Object::starLineList.line[i].point[j];
					foundPoint = true;
					lineIndex = i;
					pointIndex = (float)j;
				}
			}
		}

		if (foundPoint)
		{
			// Загружаем векторы
			XMVECTOR startPos = pos;
			XMVECTOR endPos = F2V(closestPoint);

			// 2. Рассчитываем текущее расстояние
			XMVECTOR distVector = DirectX::XMVector3Length(DirectX::XMVectorSubtract(endPos, startPos));
			float distance;
			XMStoreFloat(&distance, distVector);
			// 3. Зависимость скорости от расстояния
			const float gravitySpeed = 2.0f; // Фиксированная скорость притяжения (метров в секунду)

			// Вычисляем, какую долю от всего пути игрок должен пройти за этот кадр.
			// Формула: (Скорость * ВремяКадра) / ОставшеесяРасстояние
			float step = (gravitySpeed * 1. / 60.) / distance;

			// Прибавляем шаг к общему прогрессу
			gravity.progress += step;

			// Ограничиваем прогресс единицей
			float t = min(gravity.progress, 1.0f);

			// Опционально: оставляем кубическое сглаживание для эффекта разгона
			// Если нужно абсолютно линейное движение с одинаковой скоростью — удалите эту строчку и используйте просто t
			gravity.acceleratedT = t * t;

			// 4. Интерполяция положения
			XMVECTOR newPos = DirectX::XMVectorLerp(startPos, endPos, gravity.acceleratedT);

			// Сохраняем результат обратно в структуру игрока
			pos = newPos;

			if (gravity.acceleratedT > 0.99f)
			{
				gravity.mode = false;
				gravity.progress = 0.0f;

			}

		}
	}

	void Respawn()
	{
		if (!GetAsyncKeyState('R')) return;

		int range = 10;
		pos = getRandVector4()*range;
		int startLine = 0;
		int startPoint = 0;
		float4 destPoint = Object::starLineList.line[startLine].point[startPoint];
		pos += F2V(Object::starLineList.line[startLine].point[startPoint]);
		gravity.mode = true;
		gravity.progress = 0.0f;

	}
	
	void ProcessJump()
	{
		//if (GetActiveWindow() != hWnd) return;

		auto space = GetAsyncKeyState(VK_SPACE);

		if (space && !gravity.mode)
		{
			if (jump)
			{
				if (jumpHeight < .01) {
					jump = false;
					gravity.mode = true;
					gravity.progress = 0.0f;
				}
			}
			else {
				upBeforeJump = tUP;
				forwardBeforeJump = forward;
				jumpHeight = 1.;
				jump = true;
			}
		}

		if (!space && jump)
		{
			jump = false;
			gravity.mode = true;
			gravity.progress = 0.0f;
		}

		if (!jump && gravity.mode)
		{
			ProcessGravity();
		}

		if (jumpHeight >= .01)
		{
			pos += jumpHeight * upBeforeJump / 2. + forwardBeforeJump * speed * sign(speedFactor);
		}

		if (gravity.mode)
		{
			pos += forwardBeforeJump * speed * sign(speedFactor) * (1 - gravity.acceleratedT * gravity.acceleratedT);
		}

		jumpHeight *= .8;
		
	}

	void ProcessMove()
	{

		if (GetAsyncKeyState('W'))
		{
			speed += accel;
		}
		if (GetAsyncKeyState('S'))
		{
			speed -= accel;
		}

		if (GetAsyncKeyState('D'))
		{
			axisAngleSpeed -= axisAngleAccel * sign(speedFactor);
		}

		if (GetAsyncKeyState('A'))
		{
			axisAngleSpeed += axisAngleAccel * sign(speedFactor);
		}

		axisAngleSpeed *= autoBrake;
		axisAngleSpeed = clamp(axisAngleSpeed, -maxAxisAngleSpeed, maxAxisAngleSpeed);
		axisAngle += axisAngleSpeed;

		speed *= autoBrake;
		speed = clamp(speed, 0.f, maxSpeed);

	}

	struct {
		float Time = 0;
		float Speed = 0;
		float Accel = .1;

		void Process()
		{
			if (GetAsyncKeyState('Q'))
			{
				Speed += Accel;
			}

			if (GetAsyncKeyState('E'))
			{
				Speed -= Accel;
			}

			Speed *= .9;
			Time += Speed;

		}

	} pathControl;
};

hero_ hero;

#include "sound\tracker.h"

int precalcOfs = 0;

namespace Loop
{

	bool isPrecalc = false;

	void Precalc()
	{
		InputAsm::Set({topology::triList});
		ConstBuf::Update(ConstBuf::cBuffer::global);

		for (int i = 1 ;i < 4; i++) { 
			ConstBuf::Set((ConstBuf::cBuffer)i,ConstBuf::target::both); 
		}

		isPrecalc = true;
		precalcOfs = cmdCounter;
	}

	void frameConst()
	{
		ConstBuf::frame = {
			.time = XMFLOAT4{ (float)(timer::frameBeginTime * .01) ,(float)timer::timeCursor / SAMPLES_IN_FRAME,0,0},
			.aspect = XMFLOAT4{dx11::aspect,dx11::iaspect, 0, 0}
		};

		ConstBuf::Update(ConstBuf::cBuffer::frame);
	}

	namespace cameraMan {

		cmd(run)
		{
			reflect;
			BasicCam::setCamKey({
				.camTime = 0,
				.camType = keyType::slide,
				.eye_x = 9930,
				.eye_y = 222,
				.eye_z = 71222,
				.at_x = 756,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 254,
				.up_z = -17,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			BasicCam::setCamKey({
				.camTime = 615,
				.camType = keyType::slide,
				.eye_x = -5610,
				.eye_y = -1158,
				.eye_z = -22856,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -7,
				.up_y = 253,
				.up_z = 24,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = -3,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			BasicCam::processCam();
			
		}
	}

	void Compose()
	{
		RenderTarget::Set({texture::mainRT,0});

		BlendMode::Set({
			.mode = blendmode::off,
			.op = blendop::add
		});

		ps::output.textures.screen = texture::pBuf;
		ps::output.textures.screenMid = texture::pBufMid;
		ps::output.textures.screenLow = texture::pBufLow;
		ps::output.samplers = {
				.sam1Filter = filter::linear,
				.sam1AddressU = addr::wrap,
				.sam1AddressV = addr::wrap
		};

		vs::quad.set();
		ps::output.set();

		Drawer::NullDrawer({1,1});
	}

	void scene1()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({topology::triList});

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
		});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({cullmode::back});
		
		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME /FRAMES_PER_SECOND/Dur;

		switch (t)
		{
		case 0:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = 1236,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -110,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();


			Object::Zenith({.quality = 1});
			break;
		case 1:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1086,
				.eye_y = -1198,
				.eye_z = -499,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -163,
				.up_y = 180,
				.up_z = -75,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 70,
				.slide_z = -60,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Saggitarius({ .quality = 1 });
			break;
		case 2:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 4,
				.eye_y = 3800,
				.eye_z = 79,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 14,
				.up_y = -5,
				.up_z = 254,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 310,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();

			Object::Crab({ .quality = 1 });
			break;
		case 3:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1926,
				.eye_y = -1960,
				.eye_z = 3342,
				.at_x = 0,
				.at_y = 115,
				.at_z = 0,
				.up_x = -29,
				.up_y = 248,
				.up_z = 51,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 40,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 1,
				.fly_z = -2,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aries({ .quality = 1 });
			break;
		case 4:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 2360,
				.eye_y = -2052,
				.eye_z = -402,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 163,
				.up_y = 193,
				.up_z = -27,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -211,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Scorpio({ .quality = 1 });
			break;
		case 5:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 1015,
				.eye_y = -2195,
				.eye_z = 1015,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 150,
				.up_y = 139,
				.up_z = 150,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 250,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::LeoBigStar({ .quality = 1 });
			break;
		case 6:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -2462,
				.eye_y = -4185,
				.eye_z = 762,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -207,
				.up_y = 133,
				.up_z = 64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -120,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Virgo({ .quality = 1 });
			break;
		case 7:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 644,
				.eye_y = 1711,
				.eye_z = 940,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 119,
				.up_y = -141,
				.up_z = 175,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -50,
				.axisType = camAxis::global,
				.fly_x = 3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Capri({ .quality = 1 });
			break;
		case 8:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = -136,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 222,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -416,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Taurus({ .quality = 1 });
			//
			break;
		case 9:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 22,
				.eye_y = -958,
				.eye_z = -2169,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 1,
				.up_y = 233,
				.up_z = -102,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Libra({ .quality = 1 });
			break;
		case 10:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 622,
				.eye_y = -1022,
				.eye_z = -1822,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 38,
				.up_y = 225,
				.up_z = -113,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -40,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aquarius({ .quality = 1 });
			break;
		case 11:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -1357,
				.eye_y = 1027,
				.eye_z = -2163,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -50,
				.up_y = -236,
				.up_z = -80,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 3,
				.fly_y = 0,
				.fly_z = 10,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Fish({ .quality = 1 });
			break;
		case 12:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 57,
				.eye_y = 1236,
				.eye_z = -1975,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -3,
				.up_y = 216,
				.up_z = 135,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 10,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			break;
		}


		Compose();

	}

	void scene2()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({ topology::triList });

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;

		switch (t)
		{
		case 0:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 3,
				.eye_y = -1096,
				.eye_z = 2,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -193,
				.up_y = 0,
				.up_z = -165,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 10,
				.slide_z = -280,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 2,
				.jitter = 0
			});
			BasicCam::processCam();


			Object::Zenith({.quality = 1});
			break;
		case 1:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 26,
				.eye_y = 3002,
				.eye_z = 41,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 138,
				.up_y = 0,
				.up_z = 214,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 160,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = -3,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Saggitarius({ .quality = 1 });
			break;
		case 2:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 4,
				.eye_y = 3800,
				.eye_z = 79,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 14,
				.up_y = -5,
				.up_z = 254,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 437,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();

			Object::Crab({ .quality = 1 });
			break;
		case 3:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -170,
				.eye_y = 3689,
				.eye_z = -259,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 139,
				.up_y = 21,
				.up_z = 212,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 440,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aries({ .quality = 1 });
			break;
		case 4:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 526,
				.eye_y = -928,
				.eye_z = 286,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -188,
				.up_y = -138,
				.up_z = -102,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -511,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Scorpio({ .quality = 1 });
			break;
		case 5:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t * SAMPLES_IN_FRAME * FRAMES_PER_SECOND * Dur,
				.camType = keyType::set,
				.eye_x = 1015,
				.eye_y = -2195,
				.eye_z = 1015,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 150,
				.up_y = 139,
				.up_z = 150,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -450,
				.axisType = camAxis::global,
				.fly_x = -13,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::LeoBigStar({ .quality = 1 });
			break;
		case 6:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 0,
				.eye_y = 0,
				.eye_z = -11101,
				.at_x = 0,
				.at_y = 0,
				.at_z = 11110,
				.up_x = 244,
				.up_y = -71,
				.up_z = -5,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 2110,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Virgo({ .quality = 1 });
			break;
		case 7:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 49,
				.eye_y = 1120,
				.eye_z = 12,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -246,
				.up_y = 11,
				.up_z = -64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 22,
				.slide_y = 10,
				.slide_z = -10,
				.axisType = camAxis::local,
				.fly_x = 3,
				.fly_y = -2,
				.fly_z = -3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Capri({ .quality = 1 });
			break;
		case 8:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 49,
				.eye_y = 1120,
				.eye_z = 12,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -246,
				.up_y = 11,
				.up_z = -64,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -416,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Taurus({ .quality = 1 });
			//
			break;
		case 9:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 134,
				.eye_y = 1624,
				.eye_z = 67,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 227,
				.up_y = -23,
				.up_z = 113,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = -410,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Libra({ .quality = 1 });
			break;
		case 10:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 239,
				.eye_y = -2499,
				.eye_z = 277,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 164,
				.up_y = 37,
				.up_z = 190,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = -140,
				.axisType = camAxis::global,
				.fly_x = -3,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Aquarius({ .quality = 1 });
			break;
		case 11:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = -8,
				.eye_y = 5,
				.eye_z = -1009,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 114,
				.up_z = 0,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = 1,
				.fly_y = 2,
				.fly_z = 3,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Fish({ .quality = 1 });
			break;
		case 12:
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 10,
				.eye_y = 5111,
				.eye_z = 0,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -254,
				.up_y = 0,
				.up_z = -21,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 710,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			break;
		}


		Compose();

	}

	

	

	float4 calculate_camera_up(const float4& eye, const float4& at) {
		// 1. Вычисляем нормализованный вектор взгляда (Forward)
		float4 forward = { at.x - eye.x, at.y - eye.y, at.z - eye.z, 0.0f };
		forward = normalize(forward);

		// 2. Задаем базовый мировой вектор "верх" (обычно ось Y)
		float4 world_up = { 0.0f, 1.0f, 0.0f, 0.0f };

		// КРАЙНИЙ СЛУЧАЙ (Gimbal Lock): Если камера смотрит строго вверх или строго вниз,
		// вектор forward станет коллинеарен world_up. Векторное произведение выдаст 0.
		// В этом случае временно меняем мировой ориентир на ось Z.
		float dot = forward.x * world_up.x + forward.y * world_up.y + forward.z * world_up.z;
		if (std::abs(dot) > 0.999f) {
			world_up = float4{ 0.0f, 0.0f, 1.0f, 0.0f }; // Если смотрим вертикально, "верх" берем от Z
		}

		// 3. Находим вектор "право" (Right) через Cross Product
		float4 right = normalize(cross(forward, world_up));

		// 4. Находим финальный "верх" (Up), перпендикулярный и взгляду, и правому вектору
		float4 up = normalize(cross(right, forward));

		return up;
	}

	// 2. ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРАВЛЕНИЯ МЕЖДУ ДВУМЯ ТОЧКАМИ
	// Возвращает единичный вектор направления от точки 'from' к точке 'to'
	float4 direction_between(const float4& from, const float4& to) {
		// Находим вектор разности (вектор направления)
		float4 dir = {
			to.x - from.x,
			to.y - from.y,
			to.z - from.z,
			from.w // w-компоненту обычно наследуют от базовой точки
		};

		// Вызываем отдельную функцию нормализации
		return normalize(dir);
	}

	// Глобальные координаты камеры для связи между функциями
	XMVECTOR finalCameraEye = XMVectorSet(0.0f, 5.0f, -50.0f, 1.0f);
	XMVECTOR finalCameraAt = XMVectorZero();

	// Переменные для передачи сдвига мыши
	float mouseDeltaX = 0.0f;
	float mouseDeltaY = 0.0f;

	// Вспомогательная функция интерполяции векторов
	inline XMVECTOR VectorLerp(FXMVECTOR V1, FXMVECTOR V2, float t) {
		return XMVectorLerp(V1, V2, t);
	}

	void ProcessMouseInput(float sensitivity)
	{
		if (GetForegroundWindow() != hWnd) {
			mouseDeltaX = 0.0f;
			mouseDeltaY = 0.0f;
			return;
		}

		RECT rect;
		GetClientRect(hWnd, &rect);

		POINT windowCenter = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
		ClientToScreen(hWnd, &windowCenter);

		POINT currentMousePos;
		GetCursorPos(&currentMousePos);

		int deltaX = currentMousePos.x - windowCenter.x;
		int deltaY = currentMousePos.y - windowCenter.y;

		mouseDeltaX = (float)deltaX * sensitivity;
		mouseDeltaY = -(float)deltaY * sensitivity;

		SetCursorPos(windowCenter.x, windowCenter.y);
	}

	void UpdateHeroOnLine(float deltaTime)
	{
		// Если включен режим свободной гравитации — эта функция ничего не делает
		if (hero.gravity.mode)
		{
			// Строим матрицу вращения в свободном режиме по направлению камеры
			XMVECTOR cameraLookAtDir = XMVector3Normalize(XMVectorSubtract(finalCameraAt, finalCameraEye));
			if (XMVector3Equal(cameraLookAtDir, XMVectorZero())) cameraLookAtDir = XMVectorSet(0, 0, 1, 0);

			XMVECTOR heroForward = cameraLookAtDir;
			XMVECTOR heroUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			XMVECTOR heroRight = XMVector3Normalize(XMVector3Cross(heroUp, heroForward));
			heroUp = XMVector3Normalize(XMVector3Cross(heroForward, heroRight));

			XMMATRIX rowMajorRot = XMMatrixIdentity();
			rowMajorRot.r[0] = heroRight;
			rowMajorRot.r[1] = heroUp;
			rowMajorRot.r[2] = heroForward;

			Object::heroWorld = XMMatrixTranspose(rowMajorRot);
			return;
		}

		// Безопасная проверка границ массивов ломаной
		if (hero.lineIndex < 0 || hero.lineIndex >= Object::starLineList.lineCount) return;
		const auto& currentLine = Object::starLineList.line[hero.lineIndex];
		if (currentLine.pointCount < 2) return;

		int maxPointIdx = currentLine.pointCount - 1;
		int currIdx = clamp((int)hero.pointIndex, 0, maxPointIdx);
		int nextIdx = clamp(currIdx + 1, 0, maxPointIdx);
		int prevIdx = clamp(currIdx - 1, 0, maxPointIdx);

		// Расчет направления текущего сегмента нити
		XMVECTOR pCurrent = F2V(currentLine.point[currIdx]);
		XMVECTOR pNext = F2V(currentLine.point[nextIdx]);
		XMVECTOR lineForward = XMVector3Normalize(XMVectorSubtract(pNext, pCurrent));

		if (XMVector3Equal(lineForward, XMVectorZero()) && currIdx > 0) {
			XMVECTOR pPrev = F2V(currentLine.point[prevIdx]);
			lineForward = XMVector3Normalize(XMVectorSubtract(pCurrent, pPrev));
		}

		// Вектор взгляда камеры определяет знак направления движения (лицо/спина)
		XMVECTOR cameraLookAtDir = XMVector3Normalize(XMVectorSubtract(finalCameraAt, finalCameraEye));
		float directionSign = (XMVectorGetX(XMVector3Dot(lineForward, cameraLookAtDir)) >= 0.0f) ? 1.0f : -1.0f;

		// Смещение индекса по нити (если персонаж не оторван прыжком)
		if (!hero.jump)
		{
			float segmentLength = XMVectorGetX(XMVector3Length(XMVectorSubtract(pNext, pCurrent)));
			if (segmentLength < 0.001f) segmentLength = 1.0f;

			hero.pointIndex += (hero.speed * directionSign * deltaTime) / segmentLength;

			if (hero.pointIndex < 0.0f) { hero.pointIndex = 0.0f; hero.speed = 0.0f; }
			if (hero.pointIndex >= (float)maxPointIdx) { hero.pointIndex = (float)maxPointIdx; hero.speed = 0.0f; }

			hero.forwardBeforeJump = lineForward * directionSign;
		}

		// Расчет ортогонального базиса кручения (Френе) вокруг нити
		currIdx = clamp((int)hero.pointIndex, 0, maxPointIdx);
		nextIdx = clamp(currIdx + 1, 0, maxPointIdx);
		float t = hero.pointIndex - (float)currIdx;

		XMVECTOR posOnLine = VectorLerp(F2V(currentLine.point[currIdx]), F2V(currentLine.point[nextIdx]), t);

		XMVECTOR tangent = XMVector3Normalize(XMVectorSubtract(F2V(currentLine.point[nextIdx]), F2V(currentLine.point[currIdx])));
		if (XMVector3Equal(tangent, XMVectorZero())) tangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		XMVECTOR worldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		if (fabsf(XMVectorGetX(XMVector3Dot(tangent, worldUp))) > 0.99f) worldUp = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, tangent));
		XMVECTOR up = XMVector3Normalize(XMVector3Cross(tangent, right));

		// Поворот базиса кнопками A/D (из hero.axisAngle)
		XMMATRIX twistMatrix = XMMatrixRotationAxis(tangent, hero.axisAngle);
		up = XMVector3TransformNormal(up, twistMatrix);
		right = XMVector3TransformNormal(right, twistMatrix);

		if (!hero.jump) {
			hero.upBeforeJump = up;
		}

		// Корректируем hero.pos, если мы бежим плотно по нити
		if (!hero.jump)
		{
			hero.pos = XMVectorAdd(posOnLine, up * hero.yOffset);
		}

		// Сборка Column-Major матрицы вращения героя Object::heroWorld
		XMVECTOR heroForward = (hero.jump) ? hero.forwardBeforeJump : (tangent * directionSign);
		XMVECTOR heroUp = (hero.jump) ? hero.upBeforeJump : up;
		XMVECTOR heroRight = XMVector3Normalize(XMVector3Cross(heroUp, heroForward));

		XMMATRIX rowMajorRot = XMMatrixIdentity();
		rowMajorRot.r[0] = heroRight;
		rowMajorRot.r[1] = heroUp;
		rowMajorRot.r[2] = heroForward;

		Object::heroWorld = XMMatrixTranspose(rowMajorRot);
	}

	void UpdateCamera(float mouseX, float mouseY, float deltaTime)
	{
		static float camPitch = 0.2f;
		static float camYaw = 0.0f;
		const float camRadius = 50.0f;

		// 1. Поворот углов от мыши
		camYaw += mouseX * deltaTime;
		camPitch = clamp(camPitch + mouseY * deltaTime, -1.4f, 1.4f);

		// 2. Вычисляем идеальную целевую позицию камеры
		float cosPitch = cosf(camPitch);
		XMVECTOR targetEyePos = XMVectorSet(
			XMVectorGetX(hero.pos) + camRadius * cosPitch * sinf(camYaw),
			XMVectorGetY(hero.pos) + camRadius * sinf(camPitch),
			XMVectorGetZ(hero.pos) + camRadius * cosPitch * cosf(camYaw),
			1.0f
		);

		// ИСПРАВЛЕНИЕ: Если камера отстала от героя (например, при включении gameCam)
		// мы мгновенно привязываем её к актуальным координатам героя, убирая застывание
		XMVECTOR distVec = XMVector3Length(XMVectorSubtract(finalCameraAt, hero.pos));
		if (XMVectorGetX(distVec) > 150.0f || XMVectorGetX(distVec) == 0.0f)
		{
			finalCameraEye = targetEyePos;
			finalCameraAt = hero.pos;
		}
		else
		{
			// Стандартное плавное следование
			finalCameraEye = XMVectorLerp(finalCameraEye, targetEyePos, deltaTime * 5.0f);
			finalCameraAt = XMVectorLerp(finalCameraAt, hero.pos, deltaTime * 8.0f);
		}

		XMVECTOR currentCameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		// 3. Заполнение матриц в Column-Major
		ConstBuf::camera.world[0] = XMMatrixIdentity();

		XMMATRIX rowMajorView = XMMatrixLookAtLH(finalCameraEye, finalCameraAt, currentCameraUp);
		ConstBuf::camera.view[0] = XMMatrixTranspose(rowMajorView);

		XMMATRIX rowMajorProj = XMMatrixPerspectiveFovLH(
			DegreesToRadians(hero.cameraAngle),
			dx11::iaspect,
			0.01f,
			100.0f
		);
		ConstBuf::camera.proj[0] = XMMatrixTranspose(rowMajorProj);

		ConstBuf::Update(ConstBuf::cBuffer::camera);
		ConstBuf::Set(ConstBuf::cBuffer::camera, ConstBuf::target::both);
	}
	
	

	void scene3()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;
		frameConst();
		InputAsm::Set({ topology::triList });
		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});
		DepthBuf::Mode({ depthmode::off });
		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;
				
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = 0*SAMPLES_IN_FRAME*FRAMES_PER_SECOND*Dur,
				.camType = keyType::set,
				.eye_x = 43,
				.eye_y = -52,
				.eye_z = 1000,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = 0,
				.up_y = 100,
				.up_z = 0,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 0,
				.axisType = camAxis::global,
				.fly_x = -18,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
			});

			if (!gameCam) BasicCam::processCam();
			
			RenderTarget::Set({ texture::pBuf,0 });
			RenderTarget::Clear({ 0,0,0,0 });
			DepthBuf::Clear({});

			RenderTarget::Set({ texture::pBufMid,0 });
			RenderTarget::Clear({ 0,0,0,0 });

			RenderTarget::Set({ texture::pBufLow,0 });
			RenderTarget::Clear({ 0,0,0,0 });

			RenderTarget::Set({ texture::pBuf,0 });
			
			//

			Object::initPatches(hero.pathControl.Time);

			if (GetActiveWindow() == hWnd && gameCam)
			{
				// 1. Считываем ввод мыши
				ProcessMouseInput(0.1f);

				// 2. Выполняются ваши базовые методы (не трогаем их код)
				hero.Respawn();
				hero.ProcessMove(); // Изменяет скорости и углы
				hero.ProcessJump(); // Обрабатывает прыжок и свободную гравитацию

				// Вспомогательный вызов, который вы убрали
				hero.pathControl.Process();

				// 3. НОВЫЙ ВЫЗОВ: Рассчитывает движение по нити и Object::heroWorld
				UpdateHeroOnLine(1.f / 60.f);

				// 4. Обновляем камеру (она мгновенно подхватит hero.pos без застываний)
				UpdateCamera(mouseDeltaX, mouseDeltaY, 1.f / 60.f);
			}

			/*
			{
				hero.pointIndex += hero.speed * hero.speedFactor;
				hero.pointIndex = clamp(hero.pointIndex, 1., (float)(Object::starLineList.line[hero.lineIndex].pointCount - 1));

				float t0 = floor(hero.pointIndex);
				float t1 = frac(hero.pointIndex);

				float4 current_node = Object::starLineList.line[hero.lineIndex].point[(int)t0];
				float4 next_node = Object::starLineList.line[hero.lineIndex].point[(int)t0 + 1];
				//движение по пути
				float4 follow_node = Object::starLineList.line[hero.lineIndex].point[(int)t0 - 1];

				if (!hero.gravity.mode && !hero.jump)
				{
					hero.pos = F2V(lerp3(current_node, next_node, t1));

				}

				follow = lerp3(follow_node, current_node, t1);
				static bool isMouseInitialized = false;
				if (GetActiveWindow() == hWnd && gameCam) // Считаем мышь, только если окно в фокусе
				{
					float cd = 3.0f; // Дистанция до героя

					// --- ПЕРЕМЕННЫЕ ДЛЯ ЗАПАЗДЫВАНИЯ КАМЕРЫ И МЫШИ (static) ---
					static XMVECTOR lastUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
					static XMVECTOR smoothedLineEye = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
					static XMVECTOR smoothedLineUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
					static XMVECTOR smoothedPlayerUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

					static float mouseYaw = 0.0f;
					static float mousePitch = 0.0f;


					// --- НАСТРОЙКИ СГЛАЖИВАНИЯ И ЧУВСТВИТЕЛЬНОСТИ МЫШИ ---
					float deltaTime = 0.016f;        // Реальный dt кадра
					float distanceLerpFactor = 7.0f;
					float trackAngularLerpFactor = 5.0f;
					float playerAngularLerpFactor = 12.0f;

					float mouseSensitivity = 0.002f; // Чувствительность мыши (в радианах на пиксель)

					// ==========================================
					// ЭТАП 0: ЗАХВАТ И РАСЧЕТ ДЕЛЬТ МЫШИ (ЧЕРЕЗ WIN32)
					// ==========================================
					{
						RECT rect;
						GetWindowRect(hWnd, &rect);

						// Находим центр окна
						int centerX = rect.left + (rect.right - rect.left) / 2;
						int centerY = rect.top + (rect.bottom - rect.top) / 2;

						POINT currentPos;
						if (GetCursorPos(&currentPos))
						{
							if (!isMouseInitialized)
							{
								// В самый первый кадр просто сбрасываем мышь в центр без расчета дельт,
								// иначе камеру мгновенно улетит в сторону из-за старых координат курсора.
								SetCursorPos(centerX, centerY);
								isMouseInitialized = true;
							}
							else
							{
								// Считаем разницу между центром и текущим положением
								int deltaX = currentPos.x - centerX;
								int deltaY = currentPos.y - centerY;

								// Накапливаем углы (инвертируем Y, если нужно классическое управление)
								mouseYaw += (float)deltaX * mouseSensitivity;
								mousePitch += (float)deltaY * mouseSensitivity; // Минус, чтобы мышь вверх поднимала взгляд

								// Ограничиваем Pitch (взгляд вверх/вниз), чтобы камеру не вывернуло наизнанку (ограничение ~85 градусов)
								float pitchLimit = 1.48f / 2.; // В радианах (85 градусов)
								mousePitch = clamp(mousePitch, -pitchLimit, pitchLimit);

								// Возвращаем курсор обратно в центр для следующего кадра
								SetCursorPos(centerX, centerY);
							}
						}
					}

					// Расчет весов интерполяции для каждого компонента
					float t_dist = 1.0f - std::exp(-distanceLerpFactor * deltaTime);
					float t_track = 1.0f - std::exp(-trackAngularLerpFactor * deltaTime);
					float t_play = 1.0f - std::exp(-playerAngularLerpFactor * deltaTime);

					if (!hero.jump)
					{
						float jAmp = .1;
						float jv = pow(hero.gravity.acceleratedT, 2);
						jv *= jAmp;

						p0 = XMVectorLerp(p0, XMVectorSet(follow.x, follow.y, follow.z, 0.0f), jv);

						curent_i = lerp3(current_node, next_node, t1);
						p1 = XMVectorLerp(p1, XMVectorSet(curent_i.x, curent_i.y, curent_i.z, 0.0f), jv);

						XMVECTOR forward = XMVector3Normalize(p1 - p0);
						hero.forward = XMVectorLerp(hero.forward, forward, jv);
					}
					else
					{
						float jAmp = .1;
						float jv = pow(1 - hero.jumpHeight, 2);
						jv *= jAmp;

						p0 = XMVectorLerp(p0, XMVectorSet(follow.x, follow.y, follow.z, 0.0f), jv);
						p1 = XMVectorLerp(p1, hero.pos, jv);

						curent_i = lerp3(current_node, next_node, t1);

						XMVECTOR forward = XMVector3Normalize(p1 - p0);
						hero.forward = XMVectorLerp(hero.forward, forward, jv);
					}

					// ==========================================
					// ЭТАП 1: ПАРАЛЛЕЛЬНЫЙ ПЕРЕНОС ФРЕЙМА НИТИ
					// ==========================================
					XMVECTOR dotResult = XMVector3Dot(lastUp, hero.forward);
					XMVECTOR upOnForward = XMVectorMultiply(hero.forward, dotResult);
					XMVECTOR lineUp = XMVector3Normalize(XMVectorSubtract(lastUp, upOnForward));

					if (XMVector3Less(XMVector3LengthEst(lineUp), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f))) {
						XMVECTOR alternative = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
						XMVECTOR right = XMVector3Normalize(XMVector3Cross(alternative, hero.forward));
						lineUp = XMVector3Normalize(XMVector3Cross(hero.forward, right));
					}
					lastUp = lineUp;

					// ==========================================
					// ЭТАП 2: ВРАЩЕНИЕ ВОКРУГ ОСИ НИТИ (МГНОВЕННЫЙ ЦЕЛЕВОЙ UP ГЕРОЯ)
					// ==========================================
					XMVECTOR rotationQuat = XMQuaternionRotationAxis(hero.forward, hero.axisAngle);
					XMVECTOR targetUp = XMVector3Normalize(XMVector3Rotate(lineUp, rotationQuat));

					// ==========================================
					// ЭТАП 3: МАТРИЦА ГЕРОЯ (МГНОВЕННАЯ, COLUMN-MAJOR)
					// ==========================================
					XMVECTOR HeroRight = XMVector3Normalize(XMVector3Cross(targetUp, hero.forward));
					XMVECTOR HeroRealUp = XMVector3Normalize(XMVector3Cross(hero.forward, HeroRight));
					XMVECTOR HeroPosition = XMVectorSet(0, 0, 0, 1.0f);
					tUP = HeroRealUp;

					Object::heroWorld = XMMatrixTranspose(XMMATRIX(
						XMVectorSet(XMVectorGetX(HeroRight), XMVectorGetX(HeroRealUp), XMVectorGetX(hero.forward), 0.0f),
						XMVectorSet(XMVectorGetY(HeroRight), XMVectorGetY(HeroRealUp), XMVectorGetY(hero.forward), 0.0f),
						XMVectorSet(XMVectorGetZ(HeroRight), XMVectorGetZ(HeroRealUp), XMVectorGetZ(hero.forward), 0.0f),
						XMVectorSet(XMVectorGetX(HeroPosition), XMVectorGetY(HeroPosition), XMVectorGetZ(HeroPosition), 1.0f)
					));

					// ==========================================
					// ЭТАП 4: СГЛАЖИВАНИЕ ДВИЖЕНИЯ ВДОЛЬ ОСИ НИТИ
					// ==========================================
					XMVECTOR targetLineEye = p1 - hero.forward * cd;
					smoothedLineEye = XMVectorLerp(smoothedLineEye, targetLineEye, t_dist);

					// ==========================================
					// ЭТАП 5: РАЗДЕЛЬНОЕ СГЛАЖИВАНИЕ ОРИЕНТАЦИЙ
					// ==========================================
					smoothedLineUp = XMVector3Normalize(XMVectorLerp(smoothedLineUp, lineUp, t_track));
					XMVECTOR targetPlayerUp = XMVector3Normalize(XMVector3Rotate(smoothedLineUp, rotationQuat));
					smoothedPlayerUp = XMVector3Normalize(XMVectorLerp(smoothedPlayerUp, targetPlayerUp, t_play));

					XMVECTOR currentCameraUp = smoothedPlayerUp;

					// ==========================================
					// ЭТАП 6: РАСЧЕТ ИНТЕНСИВНОСТИ ОСМОТРА МЫШЬЮ ОТ СКОРОСТИ
					// ==========================================
					float currentSpeedAbs = std::abs(hero.speed);
					float maxSpeed = max(hero.maxSpeed, 0.0001f);

					float mouseInfluenceFactor = 1.0f - (currentSpeedAbs / maxSpeed);
					mouseInfluenceFactor = clamp(mouseInfluenceFactor, 0.0f, 1.0f);

					float finalYaw = mouseYaw * mouseInfluenceFactor;
					float finalPitch = mousePitch * mouseInfluenceFactor;

					// ==========================================
					// ЭТАП 7: ФОРМИРОВАНИЕ ТОЧЕК КАМЕРЫ ЧЕРЕЗ ЗАНИЖЕННЫЙ ЦЕНТР ВРАЩЕНИЯ (PIVOT)
					// ==========================================

					// 1. Создаем виртуальную точку опоры (Pivot). 
					// Мы опускаем центр вращения камеры НИЖЕ ног персонажа по вектору currentCameraUp.
					// За счет этого персонаж визуально поднимется вверх и окажется ровно на нижней трети экрана!
					float pivotOffset = -1.8f; // Подберите это значение: больше число = персонаж ниже на экране
					XMVECTOR cameraPivot = p1 - currentCameraUp * pivotOffset;

					// 2. Находим базовую позицию камеры (без мыши) относительно этой точки опоры
					XMVECTOR baseCameraEye = smoothedLineEye + currentCameraUp * 2.0f;

					// 3. Вычисляем базовый вектор смещения от заниженного центра опоры
					XMVECTOR baseOffsetFromPivot = XMVectorSubtract(baseCameraEye, cameraPivot);

					// 4. Строим локальные оси камеры для правильного наложения мыши
					XMVECTOR camForward = XMVector3Normalize(XMVectorNegate(baseOffsetFromPivot)); // Направление строго на Pivot
					XMVECTOR camRight = XMVector3Normalize(XMVector3Cross(currentCameraUp, camForward));
					XMVECTOR camUp = XMVector3Normalize(XMVector3Cross(camForward, camRight));

					// 5. Создаем кватернионы вращения мыши вокруг локальных осей
					XMVECTOR mouseYawQuat = XMQuaternionRotationAxis(camUp, finalYaw);
					XMVECTOR mousePitchQuat = XMQuaternionRotationAxis(camRight, finalPitch);
					XMVECTOR mouseCombinedQuat = XMQuaternionMultiply(mousePitchQuat, mouseYawQuat);

					// 6. Вращаем вектор смещения вокруг нашей виртуальной точки опоры
					XMVECTOR rotatedOffsetFromPivot = XMVector3Rotate(baseOffsetFromPivot, mouseCombinedQuat);

					// 7. ФИНАЛЬНАЯ ПОЗИЦИЯ КАМЕРЫ НА СФЕРЕ ОБЛЕТА
					XMVECTOR finalCameraEye = XMVectorAdd(cameraPivot, rotatedOffsetFromPivot);

					// 8. ТОЧКА НАЗНАЧЕНИЯ (Куда смотрим)
					// Камера всегда жестко и стабильно смотрит в центр вращения cameraPivot.
					XMVECTOR finalCameraAt = cameraPivot;

					XMVECTOR camForwardVec = XMVector3Normalize(finalCameraAt - finalCameraEye);
					hero.speedFactor = XMVectorGetX(XMVector3Dot(camForwardVec, hero.forward));
					hero.speedFactor = sign(hero.speedFactor) * pow(abs(hero.speedFactor), .125);

					// ==========================================
					// ЭТАП 8: СБОРКА МАТРИЦЫ ВИДА
					// ==========================================
					ConstBuf::camera = {
						.world = XMMatrixIdentity(),
						.view = XMMatrixTranspose(XMMatrixLookAtLH(finalCameraEye, finalCameraAt, currentCameraUp)),
						.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(DegreesToRadians(hero.cameraAngle), dx11::iaspect, 0.01f, 100.0f))
					};

					ConstBuf::Update(ConstBuf::cBuffer::camera);
					ConstBuf::Set(ConstBuf::cBuffer::camera, ConstBuf::target::both);
				}
				else
				{
					// Если окно потеряло фокус (например, Alt+Tab), сбрасываем инициализацию
					isMouseInitialized = false;
				}
			}
			*/
			//


			//
			Object::HeroMesh.Load("..//fx//projectFiles//hero.obj");
			Object::MeshPtr = &Object::HeroMesh;
			float4 p = V2F(hero.pos * 100);

			Object::Mesh({
					.quality = 1,
					.xPos = (int)(p.x),
					.yPos = (int)(p.y),
					.zPos = (int)(p.z),
					.brightness = 14,
					.tickness = 2,
					.stencil = switcher::on,
					.zoom = -81
				});

			//Object::heroWorld = XMMatrixTranspose(XMMatrixIdentity());

			/*Object::BossMesh.Load("..//fx//projectFiles//edged.obj");
			Object::MeshPtr = &Object::BossMesh;
			Object::Mesh({
				.quality = 1,
				.xPos = 0,
				.yPos = 0,
				.zPos = 0,
				.brightness = 114,
				.tickness = 2,
				.stencil = switcher::on,
				.zoom = 100
				});*/
				

			Object::hero_pos = V2F(hero.pos);
			Object::Girl({
					.quality = 1,
					.xPos = 0,
					.yPos = 0,
					.zPos = 0,
					.brightness = 19,
					.tickness = 2,
					.stencil = switcher::on
				});



		Compose();

	}

	void scene4()
	{
		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();

		InputAsm::Set({ topology::triList });

		BlendMode::Set({
			.mode = blendmode::on,
			.op = blendop::add
			});


		DepthBuf::Mode({ depthmode::off });

		Culling::Set({ cullmode::back });

		//cameraMan::run({});

		int Dur = 20;
		int t = timer::timeCursor / SAMPLES_IN_FRAME / FRAMES_PER_SECOND / Dur;

		
			BasicCam::camCounter = 0;
			BasicCam::setCamKey({
				.camTime = t * SAMPLES_IN_FRAME * FRAMES_PER_SECOND * Dur,
				.camType = keyType::set,
				.eye_x = 10,
				.eye_y = 5111,
				.eye_z = 0,
				.at_x = 0,
				.at_y = 0,
				.at_z = 0,
				.up_x = -254,
				.up_y = 0,
				.up_z = -21,
				.angle = 100,
				.sType = sliderType::follow,
				.slide_x = 0,
				.slide_y = 0,
				.slide_z = 710,
				.axisType = camAxis::global,
				.fly_x = 0,
				.fly_y = 0,
				.fly_z = 0,
				.jitter = 0
				});
			BasicCam::processCam();
			Object::Twins({ .quality = 1 });
			//break;
		


		Compose();

	}

	void ProcessTrack()
	{
		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;

		frameConst();
		
		RenderTarget::Set({texture::mainRT,0});
		RenderTarget::Clear({ 11, 11, 11, 255 });
		DepthBuf::Clear({});
		tracker::Music();
			
	}


	void mainLoop()
	{
		
		scene3();
		//track__();

	}

}
