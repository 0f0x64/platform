

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

	XMVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMVECTOR forward = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR upBeforeJump = { 0.0f, 1.0f, 0.0f, 0.0f };
	XMVECTOR forwardBeforeJump = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR lineTangent = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR landingUp = { 0.0f, 0.0f, 1.0f, 0.0f };
	int lineIndex = 0;
	float pointIndex = 0;

	float speedFactor = 1;
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

	float yOffset = 0;

	struct {
		bool mode = true;
		float progress = 0.0f;
		float acceleratedT = 0;
	} gravity;

	void SpreadLandingUpVector(int lineIndex, int landingPointIdx, XMVECTOR correctLandingUp)
{
	auto& line = Object::starLineList.line[lineIndex];
	if (line.pointCount < 2) return;

	int maxIdx = line.pointCount - 1;
	landingPointIdx = clamp(landingPointIdx, 0, maxIdx);

	// Строим чистый рельсовый верх в точке приземления
	XMVECTOR pL0 = F2V(line.point[landingPointIdx]);
	XMVECTOR pL1 = F2V(line.point[landingPointIdx == maxIdx ? maxIdx - 1 : landingPointIdx + 1]);
	XMVECTOR tLanding = XMVector3Normalize(XMVectorSubtract(pL1, pL0));
	
	XMVECTOR refUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	if (fabsf(XMVectorGetX(XMVector3Dot(tLanding, refUp))) > 0.95f) refUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR baseUpLanding = XMVector3Normalize(XMVector3Cross(tLanding, XMVector3Normalize(XMVector3Cross(refUp, tLanding))));

	// === ЧЕСТНЫЙ РАСЧЕТ КВАТЕРНИOНА ПOВOРOТА МЕЖДУ ДВУМЯ ВЕКТOРАМИ ===
	XMVECTOR vFrom = baseUpLanding;
	XMVECTOR vTo = XMVector3Normalize(correctLandingUp);
	
	float dotProd = clamp(XMVectorGetX(XMVector3Dot(vFrom, vTo)), -1.0f, 1.0f);
	XMVECTOR deltaQuat;

	if (dotProd > 0.9999f)
	{
		deltaQuat = XMQuaternionIdentity();
	}
	else if (dotProd < -0.9999f)
	{
		// Векторы противоположны, ищем любую ортогональную ось для разворота на 180 градусов
		XMVECTOR orthoAxis = XMVector3Cross(vFrom, XMVectorSet(1, 0, 0, 0));
		if (XMVectorGetX(XMVector3LengthSq(orthoAxis)) < 0.001f) orthoAxis = XMVector3Cross(vFrom, XMVectorSet(0, 1, 0, 0));
		deltaQuat = XMQuaternionRotationAxis(XMVector3Normalize(orthoAxis), XM_PI);
	}
	else
	{
		XMVECTOR crossAxis = XMVector3Cross(vFrom, vTo);
		float s = sqrtf((1.0f + dotProd) * 2.0f);
		float invS = 1.0f / s;
		deltaQuat = XMVectorSet(
			XMVectorGetX(crossAxis) * invS,
			XMVectorGetY(crossAxis) * invS,
			XMVectorGetZ(crossAxis) * invS,
			s * 0.5f
		);
	}

	// Раскидываем этот кватернион по всей линии
	for (int i = 0; i <= maxIdx; ++i)
	{
		XMVECTOR pCurr = F2V(line.point[i]);
		XMVECTOR pNext = F2V(line.point[i == maxIdx ? maxIdx - 1 : i + 1]);
		XMVECTOR tangent = XMVector3Normalize(XMVectorSubtract(pNext, pCurr));
		if (i == maxIdx) tangent = XMVector3Normalize(XMVectorSubtract(pCurr, pNext));
		if (XMVector3Equal(tangent, XMVectorZero())) tangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		// Базовый "слепой" верх для текущей точки
		XMVECTOR localRefUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		if (fabsf(XMVectorGetX(XMVector3Dot(tangent, localRefUp))) > 0.95f) localRefUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		XMVECTOR localBaseUp = XMVector3Normalize(XMVector3Cross(tangent, XMVector3Normalize(XMVector3Cross(localRefUp, tangent))));

		// Коэффициент угасания закрутки (1.0f держит космический угол жестко по всей линии)
		float nodeDistance = fabsf((float)(i - landingPointIdx));
		float blendFactor = clamp(1.0f - (nodeDistance / (float)line.pointCount), 0.0f, 1.0f);

		// Если ты хочешь, чтобы угол приземления вообще не угасал к концам рельса, раскомментируй строку ниже:
		blendFactor = 1.0f;

		XMVECTOR blendedQuat = XMQuaternionSlerp(XMQuaternionIdentity(), deltaQuat, blendFactor);
		XMVECTOR finalVertexUp = XMVector3Normalize(XMVector3Rotate(localBaseUp, blendedQuat));

		// Финальная жесткая ортогонализация под 90 градусов к рельсу
		finalVertexUp = XMVector3Normalize(XMVectorSubtract(finalVertexUp, tangent * XMVectorGetX(XMVector3Dot(finalVertexUp, tangent))));

		line.upVector[i] = V2F(finalVertexUp);
	}
}

	XMVECTOR CalculateAndSpreadLandingUp(XMVECTOR startPos, XMVECTOR endPos, int lineIdx, int pointIdx)
	{
		// 1. Получаем доступ к конкретной линии
		const auto& targetLine = Object::starLineList.line[lineIdx];
		int maxPointIdx = targetLine.pointCount - 1;

		// Ограничиваем индекс сегмента для безопасности
		int currIdx = clamp(pointIdx, 0, maxPointIdx - 1);
		int nextIdx = currIdx + 1;

		// Используем твой F2V для конвертации вершин в XMVECTOR
		XMVECTOR pCurrentVertex = F2V(targetLine.point[currIdx]);
		XMVECTOR pNextVertex = F2V(targetLine.point[nextIdx]);

		// 2. Считаем честный геометрический тангенс сегмента
		XMVECTOR segmentTangent = XMVector3Normalize(XMVectorSubtract(pNextVertex, pCurrentVertex));
		if (XMVector3Equal(segmentTangent, XMVectorZero())) {
			segmentTangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		}

		// 3. Находим чистый вектор от точки приземления к исходной позиции в воздухе
		XMVECTOR rawLandingUp = XMVectorSubtract(startPos, endPos);

		// 4. Ортогонализация Грэма-Шмидта под 90 градусов
		XMVECTOR landingProjection = XMVector3Dot(rawLandingUp, segmentTangent);
		XMVECTOR cleanLandingUp = XMVectorSubtract(rawLandingUp, XMVectorMultiply(segmentTangent, landingProjection));

		// 5. Защита от деления на ноль
		if (XMVector3Less(XMVector3LengthEst(cleanLandingUp), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f)))
		{
			XMVECTOR defaultWorldUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
			if (fabsf(XMVectorGetX(XMVector3Dot(segmentTangent, defaultWorldUp))) > 0.99f) {
				defaultWorldUp = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
			}
			cleanLandingUp = XMVector3Normalize(XMVector3Cross(segmentTangent, XMVector3Normalize(XMVector3Cross(defaultWorldUp, segmentTangent))));
		}

		// Нормализуем финальный перпендикуляр приземления
		XMVECTOR finalLandingUp = XMVector3Normalize(cleanLandingUp);

		// 6. Запускаем волну распространения по ломаной линии
		SpreadLandingUpVector(lineIdx, currIdx, finalLandingUp);

		return finalLandingUp;
	}
	
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

			landingUp = CalculateAndSpreadLandingUp(startPos, endPos, lineIndex, pointIndex);

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
			pos += jumpHeight * upBeforeJump / 2. + forwardBeforeJump * speed;
		}

		if (gravity.mode)
		{
			pos += forwardBeforeJump * speed * (1 - gravity.acceleratedT * gravity.acceleratedT);
		}

		jumpHeight *= .8;
		
	}

	void ProcessMove()
	{
		bool pressingMove = false;

		if (GetAsyncKeyState('W'))
		{
			// Плавный разгон вперед с учетом знака камеры
			speed += accel * sign(speedFactor);
			pressingMove = true;
		}
		if (GetAsyncKeyState('S'))
		{
			// Плавный разгон назад с учетом знака камеры
			speed -= accel * sign(speedFactor);
			pressingMove = true;
		}

		// Если ни одна кнопка движения не нажата — плавно тормозим (автобрейк)
		if (!pressingMove)
		{
			speed *= autoBrake;
			// Мягкое зануление совсем маленькой скорости, чтобы персонаж не полз бесконечно
			if (fabsf(speed) < 0.001f) speed = 0.0f;
		}

		// Жесткий зажим скорости в максимальные рамки
		speed = clamp(speed, -maxSpeed, maxSpeed);

		// --- Логика вращения вокруг нити (A / D) ---
		bool pressingRotation = false;
		if (GetAsyncKeyState('A'))
		{
			axisAngleSpeed -= axisAngleAccel*sign(speedFactor);
			pressingRotation = true;
		}
		if (GetAsyncKeyState('D'))
		{
			axisAngleSpeed += axisAngleAccel * sign(speedFactor);
			pressingRotation = true;
		}

		if (!pressingRotation)
		{
			axisAngleSpeed *= autoBrake;
			if (fabsf(axisAngleSpeed) < 0.001f) axisAngleSpeed = 0.0f;
		}

		axisAngleSpeed = clamp(axisAngleSpeed, -maxAxisAngleSpeed, maxAxisAngleSpeed);
		axisAngle += axisAngleSpeed;
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

	// Глобальные координаты камеры
	XMVECTOR finalCameraEye = XMVectorSet(0.0f, 5.0f, -25.0f, 1.0f);
	XMVECTOR finalCameraAt = XMVectorZero();
	XMVECTOR finalCameraUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// НАКОПЛЕННЫЕ УГЛЫ МЫШИ ИЗ ВАШЕГО БЛОКА
	float mouseYaw = 0.0f;
	float mousePitch = 0.0f;
	float mouseSensitivity = 0.0015f; // Чувствительность мыши
	bool isMouseInitialized = false;  // Флаг защиты от стартового рывка

	// Сглаживание декомпозиции
	XMVECTOR smoothedHeroPos = XMVectorZero();
	XMVECTOR smoothedHeroRotQ = XMQuaternionIdentity();
	bool cameraFirstFrame = true;

	inline XMVECTOR VectorLerp(FXMVECTOR V1, FXMVECTOR V2, float t) {
		return XMVectorLerp(V1, V2, t);
	}

	void ProcessMouseInput()
	{
		if (GetForegroundWindow() != hWnd) {
			isMouseInitialized = false;
			return;
		}

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
				SetCursorPos(centerX, centerY);
				isMouseInitialized = true;
			}
			else
			{
				// Считаем разницу между центром и текущим положением
				int deltaX = currentPos.x - centerX;
				int deltaY = currentPos.y - centerY;

				// =============================================================
				// ИНВЕРСИЯ УПРАВЛЕНИЯ: Заменили += на -= для обеих дельт.
				// Теперь движение мыши полностью зеркально отражено во всех осях.
				// =============================================================
				mouseYaw -= (float)deltaX * mouseSensitivity;
				mousePitch += (float)deltaY * mouseSensitivity;

				// Ограничиваем Pitch в пределах честных 85 градусов
				float pitchLimit = 1.48f;
				mousePitch = clamp(mousePitch, -pitchLimit, pitchLimit);

				// Возвращаем курсор обратно в центр
				SetCursorPos(centerX, centerY);
			}
		}
	}

	void OrientHeroTowardsLineInAir(float deltaTime)
	{
		// Проверяем, что индекс линии валиден и она существует
		if (hero.lineIndex < 0 || hero.lineIndex >= Object::starLineList.lineCount) return;
		const auto& currentLine = Object::starLineList.line[hero.lineIndex];
		if (currentLine.pointCount < 2) return;

		int maxPointIdx = currentLine.pointCount - 1;

		// Находим индексы сегмента, над которым летим
		int currIdx = clamp((int)floorf(hero.pointIndex), 0, maxPointIdx - 1);
		int nextIdx = currIdx + 1;

		XMVECTOR pCurrent = F2V(currentLine.point[currIdx]);
		XMVECTOR pNext = F2V(currentLine.point[nextIdx]);

		// 1. Тангенс сегмента, к которому мы приближаемся
		XMVECTOR airTangent = XMVector3Normalize(XMVectorSubtract(pNext, pCurrent));
		if (XMVector3Equal(airTangent, XMVectorZero())) airTangent = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		// 2. Находим честный апвектор в воздухе (перпендикуляр от рельса к персонажу)
		float progressT = hero.pointIndex - floorf(hero.pointIndex);
		XMVECTOR projectedPosOnLine = VectorLerp(pCurrent, pNext, progressT);
		XMVECTOR rawAirUp = XMVectorSubtract(hero.pos, projectedPosOnLine);

		XMVECTOR proj = XMVector3Dot(rawAirUp, airTangent);
		XMVECTOR airUp = XMVector3Normalize(XMVectorSubtract(rawAirUp, XMVectorMultiply(airTangent, proj)));

		if (XMVector3Less(XMVector3LengthEst(airUp), XMVectorSet(0.001f, 0.001f, 0.001f, 0.001f))) {
			airUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		}

		// 3. Строим честный правый вектор
		XMVECTOR airRight = XMVector3Normalize(XMVector3Cross(airUp, airTangent));

		// 4. Собираем ЦЕЛЕВУЮ Row-Major матрицу, к которой персонаж должен развернуться
		XMMATRIX targetAirMatrix = XMMatrixIdentity();
		targetAirMatrix.r[0] = airRight;
		targetAirMatrix.r[1] = airUp;
		targetAirMatrix.r[2] = airTangent;

		XMVECTOR targetQuat = XMQuaternionRotationMatrix(targetAirMatrix);

		// 5. Извлекаем текущую ориентацию из Object::heroWorld
		XMMATRIX currentWorldRow = XMMatrixTranspose(Object::heroWorld); // Из Column в Row
		currentWorldRow.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f); // Зануляем позицию для честного Decompose

		XMVECTOR currentScale, currentQuat, currentTrans;
		XMMatrixDecompose(&currentScale, &currentQuat, &currentTrans, currentWorldRow);

		// 6. Сферическая плавная интерполяция (Slerp)
		float blendStep = clamp(deltaTime * 8.0f, 0.0f, 1.0f);
		XMVECTOR smoothQuat = XMQuaternionSlerp(currentQuat, targetQuat, blendStep);

		// Восстанавливаем финальную сглаженную матрицу вращения кадра полета
		XMMATRIX finalAirRot = XMMatrixRotationQuaternion(smoothQuat);

		// Синхронизируем системные векторы
		hero.forward = finalAirRot.r[2];
		tUP = finalAirRot.r[1];

		// Встраиваем текущую физическую позицию полета в 4-ю строку матрицы
		finalAirRot.r[3] = XMVectorSetW(hero.pos, 1.0f);

		// Отдаем в шейдер в чистом Row-Major под твой макрос mul((float3x3)model, pos)
		Object::heroWorld = finalAirRot;
	}

	void ProcessGravityMove(float deltaTime)
	{
		if (hero.gravity.mode)
		{
			// ... Если здесь выше у тебя был код изменения физической позиции hero.pos 
			// под действием гравитации (например, hero.pos += gravityVelocity * deltaTime), оставь его тут ...

			// НОВЫЙ КИНЕМАТOГРАФИЧНЫЙ РАЗВОРOТ В ВОЗДУХЕ:
			// Функция сама рассчитает плавное вращение к рельсу, привяжет hero.pos,
			// обновит tUP, hero.forward и запишет в Object::heroWorld правильный Row-Major.
			OrientHeroTowardsLineInAir(deltaTime);

			// Мгновенно выходим, чтобы старый код сборки матрицы больше ничего не затирал
			return;
		}
	}

	void UpdateHeroOnLine(float deltaTime)
	{
		// === СОСТОЯНИЕ 1: РЕЖИМ СВОБОДНОЙ ГРАВИТАЦИИ ===
		ProcessGravityMove(deltaTime);

		if (hero.gravity.mode) return;

		// === СОСТОЯНИЕ 2: ДВИЖЕНИЕ ПО НИТЯМ ===
		if (hero.lineIndex < 0 || hero.lineIndex >= Object::starLineList.lineCount) return;
		const auto& currentLine = Object::starLineList.line[hero.lineIndex];
		if (currentLine.pointCount < 2) return;

		int maxPointIdx = currentLine.pointCount - 1;

		// Фиксированный шаг времени для физики (защита от флуктуаций таймера)
		const float fixedStep = 1.0f / 60.0f;

		// Шаг по нити на основе знаковой скорости героя
		if (!hero.jump)
		{
			int currIdxCheck = clamp((int)floorf(hero.pointIndex), 0, maxPointIdx);
			int nextIdxCheck = clamp(currIdxCheck + 1, 0, maxPointIdx);
			XMVECTOR pCurrCheck = F2V(currentLine.point[currIdxCheck]);
			XMVECTOR pNextCheck = F2V(currentLine.point[nextIdxCheck]);

			float segLenCheck = XMVectorGetX(XMVector3Length(XMVectorSubtract(pNextCheck, pCurrCheck)));
			if (segLenCheck < 0.001f) segLenCheck = 1.0f;

			hero.pointIndex += (hero.speed * fixedStep * 50.f) / segLenCheck;
			hero.pointIndex = clamp(hero.pointIndex, 1.f, (float)maxPointIdx-1.);
		}

		// Вычисление индексов и непрерывной дробной части
		int currIdx = clamp((int)floorf(hero.pointIndex), 0, maxPointIdx);
		int nextIdx = clamp(currIdx + 1, 0, maxPointIdx);

		float t = hero.pointIndex - floorf(hero.pointIndex);
		if (currIdx == maxPointIdx) t = 0.0f;

		XMVECTOR pCurrent = F2V(currentLine.point[currIdx]);
		XMVECTOR pNext = F2V(currentLine.point[nextIdx]);

		// 1. Плавно интерполируем позицию на линии
		XMVECTOR posOnLine = VectorLerp(pCurrent, pNext, t);

		// ====================================================================
		// ИСПРАВЛЕНИЕ РЫВКА: НЕПРЕРЫВНЫЙ СГЛАЖЕННЫЙ ТАНГЕНС ПУТИ
		// ====================================================================
		XMVECTOR tangentCurr = XMVector3Normalize(XMVectorSubtract(pNext, pCurrent));
		if (XMVector3Equal(tangentCurr, XMVectorZero())) tangentCurr = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		int futureIdx = clamp(nextIdx + 1, 0, maxPointIdx);
		XMVECTOR tangentNext = tangentCurr;
		if (nextIdx != futureIdx) {
			XMVECTOR pFuture = F2V(currentLine.point[futureIdx]);
			tangentNext = XMVector3Normalize(XMVectorSubtract(pFuture, pNext));
			if (XMVector3Equal(tangentNext, XMVectorZero())) tangentNext = tangentCurr;
		}

		XMVECTOR tangentSmooth = XMVector3Normalize(VectorLerp(tangentCurr, tangentNext, t));
		hero.lineTangent = tangentSmooth;

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
		// УПРАВЛЕНИЕ ТВIСТОМ ВОКРУГ СГЛАЖЕННОГО ТАНГЕНСА
		// ====================================================================
		XMMATRIX twistMatrix = XMMatrixRotationAxis(tangentSmooth, -hero.axisAngle);
		XMVECTOR HeroRealUp = XMVector3TransformNormal(HeroUpBase, twistMatrix);

		// Жесткая финальная ортогонализация всей тройки осей
		HeroRealUp = XMVector3Normalize(XMVectorSubtract(HeroRealUp, tangentSmooth * XMVectorGetX(XMVector3Dot(HeroRealUp, tangentSmooth))));
		XMVECTOR HeroRight = XMVector3Normalize(XMVector3Cross(HeroRealUp, tangentSmooth));

		// Логика прыжка
		if (!hero.jump) {
			hero.upBeforeJump = HeroRealUp;
			hero.pos = XMVectorAdd(posOnLine, HeroRealUp * hero.yOffset);
			hero.forwardBeforeJump = tangentSmooth;
		}
		else {
			HeroRealUp = hero.upBeforeJump;
			HeroRight = XMVector3Normalize(XMVector3Cross(HeroRealUp, tangentSmooth));
		}

		tUP = HeroRealUp;

		XMVECTOR heroForward = tangentSmooth;
		if (hero.jump) heroForward = hero.forwardBeforeJump;

		// Сборка чистой Row-Major матрицы вращения со строгими индексами строк r
		XMMATRIX rowMajorRot = XMMatrixIdentity();
		rowMajorRot.r[0] = HeroRight;
		rowMajorRot.r[1] = HeroRealUp;
		rowMajorRot.r[2] = heroForward;
		rowMajorRot.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		// Разворот матрицы целиком на 180 градусов при переходе камеры через ноль
		float cameraLookSign = sign(hero.speedFactor);
		if (cameraLookSign < 0.0f && !hero.jump)
		{
			XMMATRIX reverseRot = XMMatrixRotationAxis(HeroRealUp, XM_PI);
			rowMajorRot = XMMatrixMultiply(rowMajorRot, reverseRot);
		}

		hero.forward = rowMajorRot.r[2];

		// Отдаем в шейдер в чистом Row-Major под твой макрос (float3x3) без заваливаний
		Object::heroWorld = rowMajorRot;
	}

	void UpdateCamera(float deltaTime)
	{
		const float camRadius = 5.0f; // Расстояние от камеры до героя

		// ====================================================================
		// СУПЕРСТАБИЛЬНЫЙ БАЗИС КРИВОЙ ДЛЯ КАМЕРЫ (ПОЛНАЯ ИЗОЛЯЦИЯ ОТ AXIS ANGLE)
		// ====================================================================
		const auto& currentLine = Object::starLineList.line[hero.lineIndex];
		int maxPointIdx = currentLine.pointCount - 1;

		XMVECTOR pathForward = hero.lineTangent; // Чистый непрерывный тангенс

		// ТОЧЕЧНОЕ ИСПРАВЛЕНИЕ: Динамический выбор апвектора для ориентации орбиты камеры
		XMVECTOR pathUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		if (hero.gravity.mode)
		{
			// Пока мы летим, целевым апвектором для кватерниона камеры становится 
			// наш вычисленный в воздухе landingUp. Камера начнет докручиваться ЗАРАНЕЕ!
			pathUp = hero.landingUp;
		}
		else
		{
			// Когда приземлились — плавно берем апвектор рельса из волнового массива
			int currIdx = clamp((int)floorf(hero.pointIndex), 0, maxPointIdx);
			int nextIdx = clamp(currIdx + 1, 0, maxPointIdx);
			float t = hero.pointIndex - floorf(hero.pointIndex);
			if (currIdx == maxPointIdx) t = 0.0f;

			XMVECTOR upCurr = F2V(currentLine.upVector[currIdx]);
			XMVECTOR upNext = F2V(currentLine.upVector[nextIdx]);
			pathUp = XMVector3Normalize(VectorLerp(upCurr, upNext, t));
		}

		// Строим Right строго по выбранному, не прыгающему pathUp
		XMVECTOR pathRight = XMVector3Normalize(XMVector3Cross(pathUp, pathForward));

		// Собираем чистую Row-Major матрицу пути (детерминант всегда +1.0)
		XMMATRIX pathMatrixRowMajor = XMMatrixIdentity();
		pathMatrixRowMajor.r[0] = pathRight;
		pathMatrixRowMajor.r[1] = pathUp; // Передает непрерывный наклон в Decompose!
		pathMatrixRowMajor.r[2] = pathForward;
		pathMatrixRowMajor.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR heroScale, heroRotQ, heroTranslation;
		XMMatrixDecompose(&heroScale, &heroRotQ, &heroTranslation, pathMatrixRowMajor);

		// Позицию берем строго из физических координат героя
		heroTranslation = hero.pos;

		// Сглаживание инерции (Защита от стартовых бросков)
		if (cameraFirstFrame)
		{
			smoothedHeroPos = heroTranslation;
			smoothedHeroRotQ = heroRotQ;
			cameraFirstFrame = false;
		}
		else
		{
			float posStep = clamp(deltaTime * 5.0f, 0.0f, 1.0f);

			// ИСПРАВЛЕНИЕ: Ставим твой мягкий коэффициент 0.1f для шёлковой докрутки
			// (При 60 FPS deltaTime * 0.6f как раз превращается в честные 0.01)
			float rotStep = clamp(deltaTime * 0.6f, 0.0f, 1.0f);

			smoothedHeroPos = XMVectorLerp(smoothedHeroPos, heroTranslation, posStep);
			// Сглаживаем стабильный геометрический кватернион, который плавно меняется еще в полете!
			smoothedHeroRotQ = XMQuaternionSlerp(smoothedHeroRotQ, heroRotQ, rotStep);
		}

		// Восстанавливаем сглаженную матрицу ориентации нити из кватерниона
		XMMATRIX mSmoothedHero = XMMatrixRotationQuaternion(smoothedHeroRotQ);
		XMVECTOR smoothedRight = mSmoothedHero.r[0];
		XMVECTOR smoothedUp = mSmoothedHero.r[1];
		XMVECTOR smoothedForward = mSmoothedHero.r[2];

		// ====================================================================
		// ЧЕСТНАЯ И МОНОЛИТНАЯ КОМПЕНСАЦИЯ AXIS ANGLE БЕЗ ДЖИТТEРА
		// ====================================================================
		XMMATRIX rotationCompMatrix = XMMatrixRotationAxis(smoothedForward, -hero.axisAngle);
		XMVECTOR compRight = XMVector3TransformNormal(smoothedRight, rotationCompMatrix);
		XMVECTOR compUp = XMVector3TransformNormal(smoothedUp, rotationCompMatrix);

		// Накладываем глобальные инвертированные углы мыши поверх скомпенсированного базиса
		float cosPitch = cosf(mousePitch);
		XMVECTOR localMouseOffset = XMVectorSet(
			cosPitch * sinf(mouseYaw),
			sinf(mousePitch),
			-cosPitch * cosf(mouseYaw),
			0.0f
		);

		// Раскладываем смещение мыши по СКОМПЕНСИРОВАННЫМ осям
		XMVECTOR worldMouseOffset = XMVectorZero();
		worldMouseOffset = XMVectorAdd(worldMouseOffset, compRight * XMVectorGetX(localMouseOffset));
		worldMouseOffset = XMVectorAdd(worldMouseOffset, compUp * XMVectorGetY(localMouseOffset));
		worldMouseOffset = XMVectorAdd(worldMouseOffset, smoothedForward * XMVectorGetZ(localMouseOffset));

		finalCameraAt = smoothedHeroPos;
		finalCameraEye = XMVectorAdd(finalCameraAt, worldMouseOffset * camRadius);

		// Вектор Up для LookAt-матрицы должен удерживать правильный наклон
		finalCameraUp = compUp;

		// СБОРКА VIEW МАТРИЦЫ С УЧЕТОМ ИНДЕКСОВ СТРОК
		XMVECTOR camForward = XMVector3Normalize(XMVectorSubtract(finalCameraAt, finalCameraEye));
		XMVECTOR camRight = XMVector3Normalize(XMVector3Cross(finalCameraUp, camForward));
		XMVECTOR exactUp = XMVector3Normalize(XMVector3Cross(camForward, camRight));

		XMMATRIX viewMatrix = XMMatrixIdentity();
		viewMatrix.r[0] = XMVectorSet(XMVectorGetX(camRight), XMVectorGetX(exactUp), XMVectorGetX(camForward), 0.0f);
		viewMatrix.r[1] = XMVectorSet(XMVectorGetY(camRight), XMVectorGetY(exactUp), XMVectorGetY(camForward), 0.0f);
		viewMatrix.r[2] = XMVectorSet(XMVectorGetZ(camRight), XMVectorGetZ(exactUp), XMVectorGetZ(camForward), 0.0f);
		viewMatrix.r[3] = XMVectorSet(
			-XMVectorGetX(XMVector3Dot(finalCameraEye, camRight)),
			-XMVectorGetX(XMVector3Dot(finalCameraEye, exactUp)),
			-XMVectorGetX(XMVector3Dot(finalCameraEye, camForward)),
			1.0f
		);

		// Запись в буфер констант
		ConstBuf::camera.world[0] = XMMatrixIdentity();
		ConstBuf::camera.view[0] = XMMatrixTranspose(viewMatrix);

		XMMATRIX rowMajorProj = XMMatrixPerspectiveFovLH(
			DegreesToRadians(hero.cameraAngle),
			dx11::iaspect,
			0.01f, 100.0f
		);
		ConstBuf::camera.proj[0] = XMMatrixTranspose(rowMajorProj);

		// Отправка обновленного буфера первой камеры на GPU
		ConstBuf::Update(ConstBuf::cBuffer::camera);
		ConstBuf::Set(ConstBuf::cBuffer::camera, ConstBuf::target::both);

		// Стабильное обновление speedFactor для следующего кадра
		hero.speedFactor = XMVectorGetX(XMVector3Dot(camForward, hero.lineTangent));
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
				static double lastFrameTime = timer::GetCounter();
				double currentFrameTime = timer::GetCounter();

				// Получаем дельту времени в секундах
				float realDeltaTime = (float)(currentFrameTime - lastFrameTime);
				lastFrameTime = currentFrameTime;

				// Защита: при самом первом старте или если realDeltaTime равен нулю, 
				// принудительно ставим стандартный шаг 1/60, чтобы не было деления на ноль.
				if (realDeltaTime <= 0.0001f) {
					realDeltaTime = 1.0f / 60.0f;
				}

				// Зажимаем сверху на 0.1 сек (100 мс), чтобы при фризах или лагах отладки 
				// физику и камеру не разрывало на гигантские расстояния
				if (realDeltaTime > 0.1f) {
					realDeltaTime = 0.1f;
				}

				// 1. Считываем ввод мыши
				ProcessMouseInput();

				// 2. Выполняются ваши базовые методы (не трогаем их код)
				hero.Respawn();
				hero.ProcessMove(); // Изменяет скорости и углы
				hero.ProcessJump(); // Обрабатывает прыжок и свободную гравитацию

				// Вспомогательный вызов, который вы убрали
				hero.pathControl.Process();

				// 3. НОВЫЙ ВЫЗОВ: Рассчитывает движение по нити и Object::heroWorld
				UpdateHeroOnLine(realDeltaTime);

				// 4. Обновляем камеру (она мгновенно подхватит hero.pos без застываний)
				UpdateCamera(realDeltaTime);
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
