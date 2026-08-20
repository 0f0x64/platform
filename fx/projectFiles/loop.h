



// helper math

XMVECTOR F2V(float4 v)
{
	return XMVECTOR{ v.x,v.y,v.z,v.w };
}

float4 V2F(XMVECTOR v)
{
	return float4{ XMVectorGetX(v),XMVectorGetY(v), XMVectorGetZ(v), XMVectorGetW(v) };
}


XMVECTOR getRandVector4()
{
	return XMVECTOR{
		getRandFloat(),
		getRandFloat(),
		getRandFloat(),
		getRandFloat()
	};
}

#include "gApi.h"
#include "camera.h"
#include "cubemap.h"
#include "object.h"



struct inputController_ {
	
	struct {
		float yaw = 0.0f;
		float pitch = 0.0f;
		float sensitivity = 0.0015f;
		bool isInitialized = false;  

		void processInput()
		{
			RECT rect;
			GetWindowRect(hWnd, &rect);

			int centerX = rect.left + (rect.right - rect.left) / 2;
			int centerY = rect.top + (rect.bottom - rect.top) / 2;

			POINT currentPos;
			if (GetCursorPos(&currentPos))
			{
				if (!isInitialized)
				{
					SetCursorPos(centerX, centerY);
					isInitialized = true;
				}
				else
				{
					int deltaX = currentPos.x - centerX;
					int deltaY = currentPos.y - centerY;
					yaw -= (float)deltaX * sensitivity;
					pitch += (float)deltaY * sensitivity;

					float pitchLimit = DegreesToRadians(85);
					pitch = clamp(pitch, -pitchLimit, pitchLimit);

					SetCursorPos(centerX, centerY);
				}
			}
		}

		XMMATRIX getLookMatrix(XMMATRIX rowMajorRot, XMVECTOR HeroRealUp, float deltaTime, float heroChangeDirSpeed)
		{

			static float yawInner = yaw;

			float targetA = -::std::round(yaw / PI) * PI;// Разворот матрицы целиком на 180 градусов при переходе камеры через ноль

			yawInner = lerp(yawInner, targetA, pow(min(deltaTime * heroChangeDirSpeed, 1.), 1.5));
			XMMATRIX reverseRot = XMMatrixRotationAxis(HeroRealUp, yawInner);
			XMMATRIX result = XMMatrixMultiply(rowMajorRot, reverseRot);

			return result;
		}

	} mouse;

	bool isForwardPressed()
	{
		return GetAsyncKeyState('W');
	}

	bool isBackwardPressed()
	{
		return GetAsyncKeyState('S');
	}

	bool isRightPressed()
	{
		return GetAsyncKeyState('D');
	}

	bool isLeftPressed()
	{
		return GetAsyncKeyState('A');
	}

	bool jumpKeyIsDown = false;
	double jumpKeyDownTime = 0.0;
	double jumpKeyUpTime = 0.0;

	bool isJumpPressed()
	{
		return GetAsyncKeyState(VK_SPACE);
	}

};

inputController_ inputController;


bool cameraFirstFrame = true;

struct hero_ {

	XMVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR posOnLine = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR forwardVector = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR upVector = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMVECTOR rightVector = { 1.0f, 0.0f, 0.0f, 0.0f };
	XMVECTOR lineTangent = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR landingUp = { 0.0f, 1.0f, 0.0f, 0.0f };

	float startAirDistance = 0;
	int lineIndex = 0;
	float pointIndex = 0;

	float speedFactor = 1;
	float speed = 0;
	float accel = 4;
	float maxSpeed = 15;
	float autoBrake = 0.95;

	float axisAngle = 0;
	float axisAngleSpeed = 0;
	float axisAngleAccel = 10;
	float maxAxisAngleSpeed = 4;
	float autoBrakeAxis = 0.4;

	float changeDirSpeed = 10;

	bool jump = false;
	float jumpHeight = 0;
	float jumpStartImpulse = 6.f;
	float jumpLandingTreshold = .01f;
	float jumpDeAccel = .98;
	
	float airProgress = 0;
	
	float yOffset = .5*1000.;

	float chargeTimer = 0.0f;
	bool isCharging = false;
	float jumpChargeProgress = 1;
	float lastJumpAmpPercent = 1;
	float jumpProgress = 1;

	float landingTimer = 0;
	float landingCoef = 0;

	float fracPointIndex = 0;

	struct {
		bool mode = true;
		float progress = 0.0f;
		float acceleratedT = 0;
		float speed = .175f;
	} gravity;

	float airSpeedAmp = 1.f;

	bool firstRun = true;
	bool respawnInProgress = true;
		
	void SpreadLandingUpVector(int lineIndex, int landingPointIdx, XMVECTOR correctLandingUp)
	{
		auto& line = Object::starLineList.line[lineIndex];
		if (line.pointCount < 2) return;

		int maxIdx = line.pointCount - 1;
		landingPointIdx = clamp(landingPointIdx, 0, maxIdx);

		// 1. Предрасчет тангенсов (направлений) для всех точек
		::std::vector<XMVECTOR> tangents(line.pointCount);
		for (int i = 0; i <= maxIdx; ++i)
		{
			XMVECTOR pCurr = F2V(line.point[i]);
			XMVECTOR pNext = F2V(line.point[i == maxIdx ? maxIdx - 1 : i + 1]);
			XMVECTOR t = (i == maxIdx) ? XMVectorSubtract(pCurr, pNext) : XMVectorSubtract(pNext, pCurr);

			if (XMVector3Equal(XMVector3LengthSq(t), XMVectorZero())) {
				tangents[i] = (i > 0) ? tangents[i - 1] : XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			}
			else {
				tangents[i] = XMVector3Normalize(t);
			}
		}

		// Лямбда для кратчайшего поворота между СОСЕДНИМИ сегментами
		auto getRotationBetweenVectors = [](XMVECTOR vFrom, XMVECTOR vTo) -> XMVECTOR
			{
				float dotProd = clamp(XMVectorGetX(XMVector3Dot(vFrom, vTo)), -1.0f, 1.0f);
				if (dotProd > 0.9999f) return XMQuaternionIdentity();

				XMVECTOR crossAxis = XMVector3Cross(vFrom, vTo);
				float s = sqrtf((1.0f + dotProd) * 2.0f);
				float invS = 1.0f / s;
				return XMVectorSet(
					XMVectorGetX(crossAxis) * invS,
					XMVectorGetY(crossAxis) * invS,
					XMVectorGetZ(crossAxis) * invS,
					s * 0.5f
				);
			};

		// 2. Инициализируем точку приземления вашим вектором
		XMVECTOR landingTangent = tangents[landingPointIdx];
		XMVECTOR lastUp = XMVector3Normalize(correctLandingUp);

		// Строгая ортогонализация под тангенс в точке приземления
		lastUp = XMVector3Normalize(XMVectorSubtract(lastUp, landingTangent * XMVectorGetX(XMVector3Dot(lastUp, landingTangent))));
		line.upVector[landingPointIdx] = V2F(lastUp);

		// 3. Распространяем вектор ВПРАВО (минимальный поворот от соседа к соседу)
		XMVECTOR currentUp = lastUp;
		for (int i = landingPointIdx + 1; i <= maxIdx; ++i)
		{
			XMVECTOR rot = getRotationBetweenVectors(tangents[i - 1], tangents[i]);
			currentUp = XMVector3Normalize(XMVector3Rotate(currentUp, rot));

			// Коррекция накопления погрешности float
			currentUp = XMVector3Normalize(XMVectorSubtract(currentUp, tangents[i] * XMVectorGetX(XMVector3Dot(currentUp, tangents[i]))));
			line.upVector[i] = V2F(currentUp);
		}

		// 4. Распространяем вектор ВЛЕВО (минимальный поворот от соседа к соседу)
		currentUp = lastUp;
		for (int i = landingPointIdx - 1; i >= 0; --i)
		{
			XMVECTOR rot = getRotationBetweenVectors(tangents[i + 1], tangents[i]);
			currentUp = XMVector3Normalize(XMVector3Rotate(currentUp, rot));

			// Коррекция накопления погрешности float
			currentUp = XMVector3Normalize(XMVectorSubtract(currentUp, tangents[i] * XMVectorGetX(XMVector3Dot(currentUp, tangents[i]))));
			line.upVector[i] = V2F(currentUp);
		}
	}

	XMVECTOR CalculateAndSpreadLandingUp(XMVECTOR startPos, XMVECTOR endPos, int lineIdx, int pointIdx)
	{
		const auto& targetLine = Object::starLineList.line[lineIdx];
		int maxPointIdx = targetLine.pointCount - 1;

		int currIdx = clamp(pointIdx, 0, maxPointIdx - 1);
		int nextIdx = currIdx + 1;

		XMVECTOR pCurrentVertex = F2V(targetLine.point[currIdx]);
		XMVECTOR pNextVertex = F2V(targetLine.point[nextIdx]);

		XMVECTOR segmentTangent = XMVector3Normalize(XMVectorSubtract(pNextVertex, pCurrentVertex));
		if (XMVector3Equal(segmentTangent, XMVectorZero())) {
			segmentTangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}

		// ====================================================================
		// ГИБРИДНОЕ ПЛАВНОЕ СМЕШИВАНИЕ (ПЛАВНЫЙ СДВИГ БАЗИСА ПРИ КАСАНИИ)
		// ====================================================================

		// 1. Считаем чистый геометрический вектор от линии к игроку
		XMVECTOR lineToHero = XMVectorSubtract(startPos, endPos);
		XMVECTOR geomLandingUp = XMVector3Normalize(lineToHero);

		// Если векторы совпали (игрок строго на линии), берем дефолт
		if (XMVector3Less(XMVector3LengthEst(lineToHero), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f))) {
			geomLandingUp = upVector;
		}

		// 2. Вычисляем текущую физическую дистанцию
		float currentDistance = XMVectorGetX(XMVector3Length(lineToHero));

		// Параметры зоны интерполяции (настрой под масштаб своего мира)
		const float START_STABILIZE_DIST = 2.5f; // На этой дистанции начинаем подмешивать upVector героя
		const float END_STABILIZE_DIST = 0.1f;   // На этой дистанции полностью переходим на upVector героя

		// Вычисляем коэффициент смешивания tBlend (от 0.0 до 1.0)
		// 0.0 — далеко (чистая геометрия дельты), 1.0 — вплотную (чистый апвектор героя)
		float tBlend = (START_STABILIZE_DIST - currentDistance) / (START_STABILIZE_DIST - END_STABILIZE_DIST);
		tBlend = clamp(tBlend, 0.0f, 1.0f);

		// Делаем кривую смешивания более мягкой у краев
		tBlend = tBlend * tBlend * (3.0f - 2.0f * tBlend);

		// 3. Смешиваем геометрический вектор с апвектором героя
		// Перед смешиванием проверяем полярность, чтобы Lerp не пошел через ноль при переворотах
		XMVECTOR targetHeroUp = upVector;
		if (XMVectorGetX(XMVector3Dot(geomLandingUp, targetHeroUp)) < 0.0f) {
			targetHeroUp = XMVectorNegate(targetHeroUp);
		}

		// Линейно интерполируем целевой вектор приземления
		XMVECTOR mixedLandingUp = XMVectorLerp(geomLandingUp, targetHeroUp, tBlend);

		// 4. Финальная ортогонализация Грэма-Шмидта относительно тангенса отрезка
		XMVECTOR landingProjection = XMVector3Dot(mixedLandingUp, segmentTangent);
		XMVECTOR cleanLandingUp = XMVectorSubtract(mixedLandingUp, XMVectorMultiply(segmentTangent, landingProjection));

		// Защита от деления на ноль
		if (XMVector3Less(XMVector3LengthEst(cleanLandingUp), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
		{
			XMVECTOR defaultWorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			if (fabsf(XMVectorGetX(XMVector3Dot(segmentTangent, defaultWorldUp))) > 0.99f) {
				defaultWorldUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			}
			cleanLandingUp = XMVector3Normalize(XMVector3Cross(segmentTangent, XMVector3Normalize(XMVector3Cross(defaultWorldUp, segmentTangent))));
		}

		XMVECTOR finalLandingUp = XMVector3Normalize(cleanLandingUp);

		// Запускаем волну распространения по ломаной линии
		SpreadLandingUpVector(lineIdx, currIdx, finalLandingUp);

		return finalLandingUp;
	}
	
	float4 projectPointToLine(float4 pos, float4 p1, float4 p2) {
		// 1. Считаем вектор линии (p2 - p1) только по 3D осям (XYZ)
		float lineX = p2.x - p1.x;
		float lineY = p2.y - p1.y;
		float lineZ = p2.z - p1.z;

		// 2. Считаем вектор от начала линии к точке pos (pos - p1)
		float posX = pos.x - p1.x;
		float posY = pos.y - p1.y;
		float posZ = pos.z - p1.z;

		// 3. Скалярное произведение (Dot Product) векторов на плоскости XYZ
		float dotProduct = posX * lineX + posY * lineY + posZ * lineZ;

		// 4. Квадрат длины отрезка линии (X^2 + Y^2 + Z^2)
		float lineLengthSq = lineX * lineX + lineY * lineY + lineZ * lineZ;

		// Защита от деления на ноль (если closestPoint и closestPoint2 совпали)
		if (lineLengthSq < 1e-6f) {
			return p1;
		}

		// 5. Вычисляем фактор проекции t
		float t = dotProduct / lineLengthSq;

		// ЕСЛИ ВАМ НУЖЕН ОТРЕЗОК (ограничить точку строго МЕЖДУ closestPoint и closestPoint2):
		// Раскомментируйте строчку ниже, чтобы точка не улетала в бесконечность за пределы рельса
		 t = ::std::clamp(t, 0.0f, 1.0f);
		 fracPointIndex = t;
		// 6. Формируем финальную 3D-точку: p1 + lineVec * t
		float4 result;
		result.x = p1.x + lineX * t;
		result.y = p1.y + lineY * t;
		result.z = p1.z + lineZ * t;
		result.w = .0f; // Гарантируем, что это корректная точка в 3D пространстве

		return result;
	}

	void ProcessGravity(float deltaTime)
	{
		float minDistance = 1e9f;
		float4 bestProjPoint = { 0,0,0,0 };
		int bestLineIndex = -1;
		float bestPointIndex = 0.0f;
		bool found = false;

		// Ищем ближайший отрезок
		for (int i = 0; i < Object::starLineList.lineCount; i++)
		{
			// Проходим по отрезкам: от j до j+1
			for (int j = 1; j < Object::starLineList.line[i].pointCount - 2; j++)
			{
				float4 p1 = Object::starLineList.line[i].point[j];
				float4 p2 = Object::starLineList.line[i].point[j + 1];

				// Проецируем текущую позицию на отрезок
				float4 proj = projectPointToLine(V2F(pos), p1, p2);

				// Считаем расстояние от игрока до его проекции на этот отрезок
				float dst = distance(V2F(pos), proj);

				if (dst < minDistance)
				{
					minDistance = dst;
					bestProjPoint = proj;
					bestLineIndex = i;
					// Точный дробный индекс на линии
					bestPointIndex = (float)j + fracPointIndex;
					found = true;
				}
			}
		}

		if (found)
		{
			lineIndex = bestLineIndex;
			pointIndex = bestPointIndex;

			// Загружаем векторы
			XMVECTOR startPos = pos;

			XMVECTOR endPos = F2V(bestProjPoint);

			landingUp = CalculateAndSpreadLandingUp(startPos, endPos, lineIndex, pointIndex);

			// Рассчитываем текущее расстояние
			XMVECTOR distVector = DirectX::XMVector3Length(DirectX::XMVectorSubtract(endPos, startPos));
			float distance;
			XMStoreFloat(&distance, distVector);
			float step = (gravity.speed * deltaTime) / distance;

			// Прибавляем шаг к общему прогрессу
			gravity.progress += step;

			// Ограничиваем прогресс единицей
			float t = clamp(gravity.progress, 0. ,1.);

			// для эффекта разгона
			gravity.acceleratedT = t * t;

			pos = DirectX::XMVectorLerp(startPos, endPos, gravity.acceleratedT);

			if (gravity.acceleratedT > 0.99f)
			{
				gravity.acceleratedT = 1.;
				gravity.mode = false;
				gravity.progress = 0.0f;
				pos = endPos;
				respawnInProgress = false;
				axisAngle = 0;
				axisAngleSpeed = 0;

			}

		}
	}

	void processLanding(float deltaTime)
	{
		landingTimer += deltaTime;

		if (gravity.mode)
		{
			landingTimer = 0;
		}

		float landindDur = .5;

		landingTimer = clamp(landingTimer, 0., landindDur);
		landingCoef = landingTimer / landindDur;
		float smoothLT = pow(smoothstep(0, 1, landingTimer / landindDur), .5);

		//присед при приземлении
		if (!gravity.mode && !isCharging)
		{
			jumpChargeProgress = 1. - .5 * sin(PI * smoothLT) * lastJumpAmpPercent;
		}

		//if (!gravity.mode && jumpChargeProgress != 1.0f) {
			ConstBuf::gltfAnim::AnimationClip& clip = ConstBuf::gltfAnim::scene.animations[1];

			clip.currentTime = (1 - jumpChargeProgress) * clip.duration;

			if (jumpChargeProgress != 1.0f) {
				ConstBuf::gltfAnim::PlayAnimation(1);
			}
			else {
				ConstBuf::gltfAnim::StopAnimation(1);
			}
		//}
	}

	void Respawn()
	{
		if (firstRun || ((!firstRun) && GetAsyncKeyState('R')))
		{
			if (!firstRun)
			{
				while (GetAsyncKeyState('R')) { Sleep(16); };
			}

			respawnInProgress = true;

			cameraFirstFrame = true;
			srand(timer::frameBeginTime);
			firstRun = false;
			int range = 20;
			pos = getRandVector4() * range;
			startAirDistance = length3(V2F(pos));
			int startLine = rand()% Object::starLineList.lineCount;
			int startPoint = rand() % (Object::starLineList.line[startLine].pointCount-2)+1;
			float4 destPoint = Object::starLineList.line[startLine].point[startPoint];
			pos += F2V(destPoint);
			gravity.mode = true;
			gravity.progress = 0.0f;

			forwardVector = XMVECTOR{ 0,0,1 };
			upVector = XMVECTOR{ 0,1,0 };
			rightVector = XMVECTOR{ 1,0,0 };
			
			lastJumpAmpPercent = 1;
		}

	}
	


	void ProcessJump(float deltaTime)
	{
		// Настройки баланса зарядки (ПЕРЕВЕДЕНЫ В МИЛЛИСЕКУНДЫ)
		const float MIN_JUMP_IMPULSE = jumpStartImpulse / 20.0f;
		const float MAX_JUMP_IMPULSE = jumpStartImpulse;

		const float MAX_CHARGE_TIME = 600.0f; // 3000 мс = 3 секунды честной зарядки
		const float SLOWDOWN_DURATION = 200.0f;  // 800 мс = 0.8 секунды на торможение
		const float SHORT_CLICK_THRESHOLD = 40.0f; // 80 мс = анатомический порог быстрого клика

		const float UNROLL_SPEED = 0.05f;
		float chargeSpeedFactor = 1.0f;

		// 1. ЛОГИКА НАКОПЛЕНИЯ ЗАРЯДА И ВЫЧИСЛЕНИЯ ЗАМЕДЛЕНИЯ
		if (!gravity.mode)
		{
			// ЕСЛИ КНОПКА ЗАЖАТА
			if (inputController.jumpKeyIsDown)
			{
				// Первый кадр нажатия кнопки
				if (!isCharging)
				{
					isCharging = true;
					chargeTimer = 0.0f;
					// Жестко синхронизируем старт с вашим миллисекундным таймером
					inputController.jumpKeyDownTime = timer::GetCounter();
				}

				// Вычисляем точное физическое время удержания в миллисекундах
				double now = timer::GetCounter();
				chargeTimer = (float)(now - inputController.jumpKeyDownTime);

				// Вычисляем чистый прогресс зарядки (от 0.0 до 1.0)
				float currentProgress = chargeTimer / MAX_CHARGE_TIME;
				if (currentProgress > 1.0f) currentProgress = 1.0f;
				lastJumpAmpPercent = currentProgress;
				currentProgress = pow(currentProgress, .5);

				// Бленд плавно опускается сообразно проценту зарядки
				jumpChargeProgress = 1.0f - currentProgress;

				// Если передержали заряд дольше максимума — плавно тормозим
				if (chargeTimer > MAX_CHARGE_TIME)
				{
					float overchargeTime = chargeTimer - MAX_CHARGE_TIME;
					float slowdownProgress = overchargeTime / SLOWDOWN_DURATION;
					if (slowdownProgress > 1.0f) slowdownProgress = 1.0f;

					chargeSpeedFactor = 1.0f - slowdownProgress * 0.9f;
				}
			}
			// ЕСЛИ КНОПКА НЕ ЗАЖАТА, НО МЫ НАХОДИЛИСЬ В ПРОЦЕССЕ ЗАРЯДКИ — ПРЫГАЕМ!
			else if (!inputController.jumpKeyIsDown && isCharging)
			{
				isCharging = false;

				// Вычисляем чистую разницу времени между кликами в миллисекундах
				float exactHoldDuration = (float)(inputController.jumpKeyUpTime - inputController.jumpKeyDownTime);

				float progress = exactHoldDuration / MAX_CHARGE_TIME;
				if (progress > 1.0f) progress = 1.0f;

				// Ваша оригинальная S-образная кривая
				float smoothProgress = progress * progress * (3.0f - 2.0f * progress);
				jumpProgress = smoothProgress;
				//float smoothProgress = progress * progress * progress;

				jumpHeight = MIN_JUMP_IMPULSE + (MAX_JUMP_IMPULSE - MIN_JUMP_IMPULSE) * smoothProgress;

				jump = true;
				gravity.mode = true;
				gravity.progress = 0.0f;

				speed *= airSpeedAmp;
			}
			else
			{
				// Обычная стойка на нити без нажатий
				jumpChargeProgress = 1.0f;
			}
		}
		else
		{
			// === В ПОЛЕТЕ: БЫСТРО РАСПРЯМЛЯЕМ ДО 1 ===
			if (jumpChargeProgress < 1.0f)
			{
				float dt = deltaTime / (1. / 60.);
				jumpChargeProgress += (1.0f - jumpChargeProgress) * (1.0f - powf(UNROLL_SPEED, dt));
				if (jumpChargeProgress > 1.0f) jumpChargeProgress = 1.0f;
			}
		}

		// 2. ВАША ОРИГИНАЛЬНАЯ ЛОГИКА ПРИЗЕМЛЕНИЯ
		//if (jumpHeight < 1.01- pow(lastJumpAmpPercent,.25)) {
		if (jumpHeight < 1) {
			jumpHeight = 0;
			jump = false;
		}

		// 3. АДАПТИРОВАННАЯ ЛОГИКА ПЕРЕМЕЩЕНИЯ
		if (jump || gravity.mode)
		{
			pos += (jumpHeight * upVector + forwardVector * speed * airSpeedAmp + rightVector*axisAngleSpeed ) * deltaTime;
		}

		// 4. ВАША ОРИГИНАЛЬНАЯ ЛОГИКА ЗАТУХАНИЯ СКОРОСТИ ПРЫЖКА
		float dt = deltaTime / (1. / 60.);
		jumpHeight *= pow(jumpDeAccel, dt);


	}

	void ProcessMove(float deltaTime)
	{
		if (jump || gravity.mode) return;

		bool pressingMove = false;
		
		if (inputController.isForwardPressed())
		{
			// Плавный разгон вперед с учетом знака камеры
			speed += accel * sign(speedFactor)*deltaTime;

			if (sign(speed) != sign(speedFactor))
			{
				speed += accel * sign(speedFactor) * deltaTime;
				float dt = deltaTime / (1. / 600.);
				//speed *= pow(autoBrake, dt);//double force for reverse
			}

			pressingMove = true;
		}
		if (inputController.isBackwardPressed())
		{
			// Плавный разгон назад с учетом знака камеры
			speed -= accel * sign(speedFactor)*deltaTime;
			pressingMove = true;
		}

		// Если ни одна кнопка движения не нажата — плавно тормозим (автобрейк)
		if (!pressingMove)
		{
			float dt = deltaTime / (1. / 6.);
			speed *= pow(autoBrake, dt);
			// Мягкое зануление совсем маленькой скорости, чтобы персонаж не полз бесконечно
			if (fabsf(speed) < 0.001f) speed = 0.0f;
		}

		if (abs(speed) > 0.5f) {
			ConstBuf::gltfAnim::PlayAnimation(2);
		}
		else {
			ConstBuf::gltfAnim::StopAnimation(2);
		}

		// Жесткий зажим скорости в максимальные рамки
		speed = clamp(speed, -maxSpeed, maxSpeed);

		// --- Логика вращения вокруг нити (A / D) ---
		bool pressingRotation = false;
		if (inputController.isLeftPressed())
		{
			axisAngleSpeed -= axisAngleAccel*sign(speedFactor) *deltaTime;
			pressingRotation = true;
		}
		if (inputController.isRightPressed())
		{
			axisAngleSpeed += axisAngleAccel * sign(speedFactor) * deltaTime;
			pressingRotation = true;
		}

		if (!pressingRotation)
		{
			float dt = deltaTime / (1. / 6.);
			axisAngleSpeed *= pow(autoBrakeAxis, dt);
			if (fabsf(axisAngleSpeed) < 0.001f) axisAngleSpeed = 0.0f;
		}

		axisAngleSpeed = clamp(axisAngleSpeed, -maxAxisAngleSpeed, maxAxisAngleSpeed);
		axisAngle += axisAngleSpeed *deltaTime;
	}

	XMVECTOR getSmoothTangent()
	{
		const auto& currentLine = Object::starLineList.line[lineIndex];
		int maxPointIdx = currentLine.pointCount - 1;
		// Вычисление индексов и непрерывной дробной части
		int currIdx = clamp((int)floorf(pointIndex), 0, maxPointIdx);
		int nextIdx = clamp(currIdx + 1, 0, maxPointIdx);

		float t = pointIndex - floorf(pointIndex);
		if (currIdx == maxPointIdx) t = 0.0f;

		XMVECTOR pCurrent = F2V(currentLine.point[currIdx]);
		XMVECTOR pNext = F2V(currentLine.point[nextIdx]);

		// ====================================================================
		// НЕПРЕРЫВНЫЙ СГЛАЖЕННЫЙ ТАНГЕНС ПУТИ
		// ====================================================================
		XMVECTOR tangentCurr = XMVector3Normalize(XMVectorSubtract(pNext, pCurrent));
		if (XMVector3Equal(tangentCurr, XMVectorZero()))
		{
			tangentCurr = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}

		int futureIdx = clamp(nextIdx + 1, 0, maxPointIdx);
		XMVECTOR tangentNext = tangentCurr;
		if (nextIdx != futureIdx) {
			XMVECTOR pFuture = F2V(currentLine.point[futureIdx]);
			tangentNext = XMVector3Normalize(XMVectorSubtract(pFuture, pNext));
			if (XMVector3Equal(tangentNext, XMVectorZero()))
			{
				tangentNext = tangentCurr;
			}
		}

		XMVECTOR tangentSmooth = XMVector3Normalize(XMVectorLerp(tangentCurr, tangentNext, t));
		lineTangent = tangentSmooth;
		return tangentSmooth;
	}

	XMMATRIX getHeroOnRailsMatrix(XMVECTOR forward, XMVECTOR up, XMVECTOR right)
	{
		XMMATRIX rowMajorRot = XMMatrixIdentity();
		rowMajorRot.r[0] = right;
		rowMajorRot.r[1] = up;
		rowMajorRot.r[2] = forward;
		rowMajorRot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		return rowMajorRot;
	}

	void OrientHeroTowardsLineInAir(float deltaTime)
	{
		// Проверяем, что индекс линии валиден и она существует
		if (lineIndex < 0 || lineIndex >= Object::starLineList.lineCount) return;
		const auto& currentLine = Object::starLineList.line[lineIndex];
		if (currentLine.pointCount < 2) return;

		int maxPointIdx = currentLine.pointCount - 1;

		XMVECTOR airTangent = getSmoothTangent();

		// 3. Вычисляем ТЕКУЩУЮ физическую дистанцию до нити приземления
		float distanceToLine = XMVectorGetX(XMVector3Length(XMVectorSubtract(pos, posOnLine)));
		if (distanceToLine < 0.001f) distanceToLine = 0.001f;

		XMVECTOR airUp = landingUp;

		// 5. Строим честный правый вектор целевого базиса
		XMVECTOR airRight = XMVector3Normalize(XMVector3Cross(airUp, airTangent));

		// 6. Собираем ЦЕЛЕВУЮ Row-Major матрицу, к которой персонаж должен быть развернут
		XMMATRIX targetAirMatrix = XMMatrixIdentity();
		targetAirMatrix.r[0] = airRight;
		targetAirMatrix.r[1] = airUp;
		targetAirMatrix.r[2] = airTangent;

		XMVECTOR targetQuat = XMQuaternionRotationMatrix(targetAirMatrix);

		// 7. Извлекаем текущую ориентацию из Object::heroWorld (чистый Row-Major без Transpose!)
		XMMATRIX currentWorldRow = Object::heroOnRails;
		currentWorldRow.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Зануляем позицию для честного Decompose

		XMVECTOR currentScale, currentQuat, currentTrans;
		XMMatrixDecompose(&currentScale, &currentQuat, &currentTrans, currentWorldRow);

		// ====================================================================
		// МАТЕМАТИЧЕСКАЯ ГАРАНТИЯ: ИНТЕРПОЛЯЦИЯ ПО ПРОГРЕССУ РАССТОЯНИЯ
		// ====================================================================
		// Вычисляем, какой процент пути от точки спауна до нити персонаж уже пролетел.
		// На старте (distanceToLine == startAirDistance) progress равен 0.0f (тело летит свободно).
		// В момент касания (distanceToLine == 0.0f) progress равен строго 1.0f.
		airProgress = 1.0f - (distanceToLine / startAirDistance);
		airProgress = pow(airProgress, 2.5);
		airProgress = clamp(airProgress, 0.0f, 1.0f);
		// Зажимаем коэффициент в рамки [0, 1]
		float blendStep = pow(gravity.acceleratedT, .85);

		// Сферическая плавная интерполяция идет строго по пройденному пути!
		XMVECTOR smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, blendStep);

		// Восстанавливаем финальную сглаженную матрицу вращения кадра полета
		XMMATRIX finalAirRot = XMMatrixRotationQuaternion(smoothQuat);

		// Синхронизируем системные векторы
		forwardVector = finalAirRot.r[2];
		upVector = finalAirRot.r[1];
		rightVector = XMVector3Normalize(XMVector3Cross(upVector, forwardVector));

		// Встраиваем текущую физическую позицию полета в 4-ю строку матрицы
		finalAirRot.r[3] = XMVectorSetW(pos, 1.0f);

		// Отдаем в шейдер в чистом Row-Major под твой макрос mul((float3x3)model, pos)
		Object::heroOnRails = finalAirRot;

		//----------------

		Object::heroWorld = inputController.mouse.getLookMatrix(finalAirRot, upVector, deltaTime, changeDirSpeed);
		axisAngle = lerp(axisAngle, 0, blendStep);
		axisAngleSpeed = lerp(axisAngleSpeed, 0, blendStep);
		//hero.axisAngle = 0;
		//hero.axisAngleSpeed = 0;

	}

	void UpdateHeroOnLine(float deltaTime)
	{
		if (lineIndex < 0 || lineIndex >= Object::starLineList.lineCount) return;

		const auto& currentLine = Object::starLineList.line[lineIndex];
		if (currentLine.pointCount < 2) return;

		int maxPointIdx = currentLine.pointCount - 2;

		int currIdxCheck = clamp((int)floorf(pointIndex), 2, maxPointIdx);
		int nextIdxCheck = clamp(currIdxCheck + 1, 0, maxPointIdx);
		XMVECTOR pCurrCheck = F2V(currentLine.point[currIdxCheck]);
		XMVECTOR pNextCheck = F2V(currentLine.point[nextIdxCheck]);

		float segLenCheck = XMVectorGetX(XMVector3Length(XMVectorSubtract(pNextCheck, pCurrCheck)));
		if (segLenCheck < 0.001f) segLenCheck = 1.0f;

		pointIndex += (speed * deltaTime) / segLenCheck;

		pointIndex = clamp(pointIndex, 2.f, (float)maxPointIdx);

		int currIdx = clamp((int)floorf(pointIndex), 0, maxPointIdx);
		int nextIdx = clamp(currIdx + 1, 0, maxPointIdx);

		XMVECTOR pCurrent = F2V(currentLine.point[currIdx]);
		XMVECTOR pNext = F2V(currentLine.point[nextIdx]);

		XMVECTOR tangentSmooth = getSmoothTangent();

		float t = pointIndex - floorf(pointIndex);
		if (currIdx == maxPointIdx) t = 0.0f;
		posOnLine = XMVectorLerp(pCurrent, pNext, t);

		// ====================================================================
		// СВЯЗЫВАНИЕ ЧЕРЕЗ СКОМПОНОВАННЫЙ МАССИВ upVector (ПЛАВНЫЙ БАЗИС КВАТЕРНИОНОВ)
		// ====================================================================
		// Извлекаем волнообразно распределенные апвекторы для левой и правой вершин отрезка
		XMVECTOR upCurr = F2V(currentLine.upVector[currIdx]);
		XMVECTOR upNext = F2V(currentLine.upVector[nextIdx]);

		// Строим жестко ортогональные тройки векторов в вершинах, привязываясь к общему tangentSmooth
		XMVECTOR rightCurr = XMVector3Normalize(XMVector3Cross(upCurr, tangentSmooth));
		XMMATRIX matRotCurr = XMMatrixIdentity();
		matRotCurr.r[0] = rightCurr; matRotCurr.r[1] = upCurr; matRotCurr.r[2] = tangentSmooth;
		XMVECTOR qCurr = XMQuaternionRotationMatrix(matRotCurr);

		XMVECTOR rightNext = XMVector3Normalize(XMVector3Cross(upNext, tangentSmooth));
		XMMATRIX matRotNext = XMMatrixIdentity();
		matRotNext.r[0] = rightNext; matRotNext.r[1] = upNext; matRotNext.r[2] = tangentSmooth;
		XMVECTOR qNext = XMQuaternionRotationMatrix(matRotNext);

		// Сферическая интерполяция кватернионов рельса (идеальная непрерывность на стыках без микро-кивков)
		XMVECTOR qSmooth = XMQuaternionSlerp(qCurr, qNext, t);
		XMMATRIX matSmooth = XMMatrixRotationQuaternion(qSmooth);

		// Достаем идеально сглаженные опорные оси
		XMVECTOR HeroRightBase = matSmooth.r[0];
		XMVECTOR HeroUpBase = matSmooth.r[1];

		// ====================================================================
		// УПРАВЛЕНИЕ ТВИСТОМ ВОКРУГ СГЛАЖЕННОГО ТАНГЕНСА
		// ====================================================================
		XMMATRIX twistMatrix = XMMatrixRotationAxis(tangentSmooth, -axisAngle);
		XMVECTOR HeroRealUp = XMVector3TransformNormal(HeroUpBase, twistMatrix);

		// Жесткая финальная ортогонализация всей тройки осей
		HeroRealUp = XMVector3Normalize(XMVectorSubtract(HeroRealUp, tangentSmooth * XMVectorGetX(XMVector3Dot(HeroRealUp, tangentSmooth))));
		XMVECTOR HeroRight = XMVector3Normalize(XMVector3Cross(HeroRealUp, tangentSmooth));

		XMVECTOR heroForward = tangentSmooth;

		upVector = HeroRealUp;
		rightVector = HeroRight;
		forwardVector = heroForward;
		pos = XMVectorLerp(pCurrent, pNext, t);

		Object::heroOnRails = getHeroOnRailsMatrix(heroForward, HeroRealUp, HeroRight);
		Object::heroWorld = inputController.mouse.getLookMatrix(Object::heroOnRails, HeroRealUp, deltaTime, changeDirSpeed);
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



struct gameCamera_ {

	float lensAngle = 100;
	float posInertion = 3;
	float rotInertion = 1;

	XMVECTOR smoothedHeroPos = XMVectorZero();
	XMVECTOR smoothedHeroRotQ = XMQuaternionIdentity();

	// Глобальные координаты камеры
	XMVECTOR finalCameraEye = XMVectorSet(0.0f, 5.0f, -25.0f, 1.0f);
	XMVECTOR finalCameraAt = XMVectorZero();
	XMVECTOR finalCameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	void constBufSet(XMMATRIX viewMatrix)
	{
		// Запись в буфер констант
		ConstBuf::camera.world[0] = XMMatrixIdentity();
		ConstBuf::camera.view[0] = XMMatrixTranspose(viewMatrix);

		XMMATRIX rowMajorProj = XMMatrixPerspectiveFovLH(
			DegreesToRadians(lensAngle),
			dx11::iaspect,
			0.01f, 100.0f
		);
		ConstBuf::camera.proj[0] = XMMatrixTranspose(rowMajorProj);

		// Отправка обновленного буфера первой камеры на GPU
		ConstBuf::Update(ConstBuf::cBuffer::camera);
		ConstBuf::Set(ConstBuf::cBuffer::camera, ConstBuf::target::both);
	}

	XMMATRIX calcCameraMatrix(XMVECTOR right, XMVECTOR up, XMVECTOR forward,XMVECTOR eye )
	{
		XMMATRIX viewMatrix = XMMatrixIdentity();
		viewMatrix.r[0] = XMVectorSet(XMVectorGetX(right), XMVectorGetX(up), XMVectorGetX(forward), 0.0f);
		viewMatrix.r[1] = XMVectorSet(XMVectorGetY(right), XMVectorGetY(up), XMVectorGetY(forward), 0.0f);
		viewMatrix.r[2] = XMVectorSet(XMVectorGetZ(right), XMVectorGetZ(up), XMVectorGetZ(forward), 0.0f);
		viewMatrix.r[3] = XMVectorSet(
			-XMVectorGetX(XMVector3Dot(eye, right)),
			-XMVectorGetX(XMVector3Dot(eye, up)),
			-XMVectorGetX(XMVector3Dot(eye, forward)),
			1.0f
		);
		return viewMatrix;
	}

	void Update(float deltaTime)
	{
		float camRadius = 2.f; // Расстояние от камеры до героя

		if (hero.respawnInProgress)
		{
			camRadius = lerp(1.5, camRadius, hero.gravity.acceleratedT);
		}

		XMVECTOR heroScale, heroRotQ, heroTranslation;
		XMMatrixDecompose(&heroScale, &heroRotQ, &heroTranslation, Object::heroOnRails);

		// Позицию берем строго из физических координат героя
		heroTranslation = hero.pos;

		// Сглаживание инерции (Защита от стартовых бросков)
		if (cameraFirstFrame || hero.respawnInProgress)
		{
			smoothedHeroPos = heroTranslation;
			smoothedHeroRotQ = heroRotQ;
			cameraFirstFrame = false;
		}
		else
		{
			// Используем экспоненту для независимости от FPS
			float posStep = 1.0f - ::std::exp(-posInertion * deltaTime);
			float rotStep = 1.0f - ::std::exp(-rotInertion * deltaTime);

			smoothedHeroPos = XMVectorLerp(smoothedHeroPos, heroTranslation, posStep);
			smoothedHeroRotQ = XMQuaternionSlerp(smoothedHeroRotQ, heroRotQ, rotStep);
		}

		// Восстанавливаем сглаженную матрицу ориентации нити из кватерниона
		XMMATRIX mSmoothedHero = XMMatrixRotationQuaternion(smoothedHeroRotQ);
		XMVECTOR smoothedRight = mSmoothedHero.r[0];
		XMVECTOR smoothedUp = mSmoothedHero.r[1];
		XMVECTOR smoothedForward = mSmoothedHero.r[2];

		// Накладываем глобальные инвертированные углы мыши поверх скомпенсированного базиса
		float cosPitch = cosf(inputController.mouse.pitch);
		XMVECTOR localMouseOffset = XMVectorSet(
			cosPitch * sinf(inputController.mouse.yaw),
			sinf(inputController.mouse.pitch),
			-cosPitch * cosf(inputController.mouse.yaw),
			0.0f
		);

		// Раскладываем смещение мыши по СКОМПЕНСИРОВАННЫМ осям
		XMVECTOR worldMouseOffset = XMVectorZero();
		worldMouseOffset = XMVectorAdd(worldMouseOffset, smoothedRight * XMVectorGetX(localMouseOffset));
		worldMouseOffset = XMVectorAdd(worldMouseOffset, smoothedUp * XMVectorGetY(localMouseOffset));
		worldMouseOffset = XMVectorAdd(worldMouseOffset, smoothedForward * XMVectorGetZ(localMouseOffset));

		finalCameraAt = smoothedHeroPos;
		finalCameraEye = XMVectorAdd(finalCameraAt, worldMouseOffset * camRadius);

		// Вектор Up для LookAt-матрицы должен удерживать правильный наклон
		finalCameraUp = smoothedUp;

		// СБОРКА VIEW МАТРИЦЫ С УЧЕТОМ ИНДЕКСОВ СТРОК
		XMVECTOR camForward = XMVector3Normalize(XMVectorSubtract(finalCameraAt, finalCameraEye));
		XMVECTOR camRight = XMVector3Normalize(XMVector3Cross(finalCameraUp, camForward));
		XMVECTOR exactUp = XMVector3Normalize(XMVector3Cross(camForward, camRight));

		// ====================================================================
		// СДВИГ ПЕРСОНАЖА НА НИЖНЮЮ ТРЕТЬ ЭКРАНА (FRAMING OFFSET)
		// ====================================================================
		float screenOffsetY = 1.335f;

		//посадка на линию
		float landingCamAmp = .2;
		float smoothLT = pow(smoothstep(0, 1, hero.landingCoef),.5);
		screenOffsetY -= sin(PI * smoothLT*2)*(1- smoothLT) * landingCamAmp * hero.lastJumpAmpPercent;

		// если камера смотрит сверху/снизу - смещаем пивот в центр - убираем оффсет. но если игрок дивигается - возвращаем (из за инерции)
		screenOffsetY = lerp(screenOffsetY, 0, (1. - pow(abs(cosPitch), 5)) * (1 - abs(hero.speed / hero.maxSpeed)));

		XMVECTOR localScreenVerticalOffset = exactUp * screenOffsetY;

		// Смещаем точку фокуса и позицию глаза камеры параллельно вдоль экрана
		finalCameraAt = XMVectorAdd(finalCameraAt, localScreenVerticalOffset);
		finalCameraEye = XMVectorAdd(finalCameraEye, localScreenVerticalOffset);

		XMMATRIX viewMatrix = calcCameraMatrix(camRight, exactUp, camForward, finalCameraEye);

		constBufSet(viewMatrix);

		// Стабильное обновление speedFactor для следующего кадра
		hero.speedFactor = XMVectorGetX(XMVector3Dot(camForward, hero.lineTangent));
	}
};

gameCamera_ gameCamera;



float processTimer()
{
	static double lastFrameTime = timer::frameBeginTime;
	double currentFrameTime = timer::frameBeginTime;

	float realDeltaTime = (float)(currentFrameTime - lastFrameTime) / 1000.f;
	lastFrameTime = currentFrameTime;

	if (realDeltaTime <= 0.0001f) {
		realDeltaTime = 1.0f / 60.0f;
	}

	return realDeltaTime;
}

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




	cmd(SetHeroParams, int accel, int maxSpeed, int autoBrake, int axisAngleAccel, int maxAxisSpeed, int changeDirSpeed,
		int jumpStartImpulse, int jumpLandingTreshold, int jumpDeAccel,  int gravitySpeed, int airSpeedAmp, int autoBrakeAxis)
	{
		reflect;
		float denom = 100;
		hero.accel = in.accel / denom;
		hero.maxSpeed = in.maxSpeed / denom;
		hero.autoBrake = in.autoBrake / denom;
		hero.axisAngleAccel = in.axisAngleAccel / denom;
		hero.maxAxisAngleSpeed = in.maxAxisSpeed / denom;
		hero.changeDirSpeed = in.changeDirSpeed / denom;
		hero.jumpStartImpulse = in.jumpStartImpulse / denom;
		hero.jumpLandingTreshold = in.jumpLandingTreshold / denom;
		hero.jumpDeAccel = in.jumpDeAccel / denom;
		hero.gravity.speed = in.gravitySpeed / denom;
		hero.airSpeedAmp = in.airSpeedAmp / denom;
		hero.autoBrakeAxis = in.autoBrakeAxis / denom;
	}

	cmd(SetCameraParams, int angle, int posInertion, int rotInertion)
	{
		reflect;

		float denom = 100;
		gameCamera.lensAngle = in.angle;
		gameCamera.posInertion = in.posInertion / denom;
		gameCamera.rotInertion = in.rotInertion / denom;
	}

	void scene3()
	{
		SetHeroParams({
			.accel = 351,
			.maxSpeed = 1925,
			.autoBrake = 65,
			.axisAngleAccel = 1220,
			.maxAxisSpeed = 800,
			.changeDirSpeed = 980,
			.jumpStartImpulse = 3053,
			.jumpLandingTreshold = 10,
			.jumpDeAccel = 90,
			.gravitySpeed = 88,
			.airSpeedAmp = 100,
			.autoBrakeAxis = 30
			});

		SetCameraParams({
			.angle = 110,
			.posInertion = 1400,
			.rotInertion = 1000
			});

		BasicCam::camPass = false;
		BasicCam::camCounter = 0;

		if (!isPrecalc)
		{
			Precalc();
		}

		cmdCounter = precalcOfs;
		frameConst();
		
			
			//

			Object::initPatches(hero.pathControl.Time);

			if (GetActiveWindow() == hWnd && gameCam)
			{

				float deltaTime = processTimer();

				inputController.mouse.processInput();

				hero.Respawn();

				const float FIXED_DT = 1.0f / 60.0f; // Строго 16.66 мс для физики
				static float accumulator = 0.0f;
				static float smoothedCameraDT = FIXED_DT;
				const float EMA_SMOOTH_FACTOR = 0.15f; // Коэффициент фильтра времени

				accumulator += deltaTime;

				if (accumulator > 0.1f) accumulator = 0.1f;

				while (accumulator >= FIXED_DT) {

					hero.pathControl.Process();
					hero.ProcessMove(FIXED_DT); 
					hero.ProcessJump(FIXED_DT); 

					if (hero.gravity.mode)
					{
						hero.ProcessGravity(FIXED_DT);
						hero.OrientHeroTowardsLineInAir(FIXED_DT);
					}
					else
					{
						hero.UpdateHeroOnLine(FIXED_DT);
					}

					hero.processLanding(FIXED_DT);

					gameCamera.Update(FIXED_DT);

					accumulator -= FIXED_DT;
				}

				ConstBuf::interp::UpdateTweens(deltaTime);

				float alpha = accumulator / FIXED_DT;
				alpha = ::std::clamp(alpha, 0.0f, 1.0f);
				//TODO: implement characters and camera matrix interpolation (render only)
			}

			if (!isPrecalc)
			{
				Precalc();
			}

			cmdCounter = precalcOfs;
			frameConst();


			//---------
			//RENDERING
			//---------

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
				.camTime = 0 * SAMPLES_IN_FRAME * FRAMES_PER_SECOND * Dur,
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
			//Object::HeroMesh.Load("..//fx//projectFiles//hero.obj");
			//Object::MeshPtr = &Object::HeroMesh;

			//ConstBuf::LoadObj("..//fx//projectFiles//Landing_Misha.glb");
			//Object::MeshPtr = nullptr;

			// --- ЗАГРУЗКА МОДЕЛИ --- //
			static bool sceneInitialized = false;
			if (!sceneInitialized)
			{
			//ConstBuf::LoadObj("..//fx//projectFiles//Landing_Misha.glb");
			ConstBuf::LoadObj("..//fx//projectFiles//A-Pose.glb");
			Object::MeshPtr = nullptr;

			sceneInitialized = true;
			}
			// ----- //

			float4 p = V2F(hero.pos * 10000.);

			Object::Mesh({
					.quality = 1,
					.xPos = (int)(p.x),
					.yPos = (int)(p.y),
					.zPos = (int)(p.z),
					.brightness = 9,
					.tickness = 4,
					.stencil = switcher::on,
					.zoom = -75,
					.onLineOfs = (int)hero.yOffset,
					.jumpCharge = 100
				});

			//.jumpCharge = (int)(hero.jumpChargeProgress*100.)

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
