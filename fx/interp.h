//#include <functional>
#include <vector>
#include <cmath>
#include <memory>
#include <algorithm>


namespace interp {

    enum class Curve {
        Linear,
        EaseInQuad,
        EaseOutQuad,
        EaseInOutQuad,
        EaseInCubic,
        EaseOutCubic,
        EaseInOutCubic,
        EaseOutElastic,
        EaseOutBounce
    };

    inline float EaseCurve(Curve curve, float t) {
        switch (curve) {
        case Curve::Linear: return t;
        case Curve::EaseInQuad: return t * t;
        case Curve::EaseOutQuad: return t * (2 - t);
        case Curve::EaseInOutQuad:
            return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
        case Curve::EaseInCubic: return t * t * t;
        case Curve::EaseOutCubic:
            return (--t) * t * t + 1;
        case Curve::EaseInOutCubic:
            return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
        case Curve::EaseOutElastic: {
            const float c4 = (2 * 3.14159f) / 3;
            return t == 0 ? 0 : t == 1 ? 1 :
                powf(2, -10 * t) * sinf((t * 10 - 0.75f) * c4) + 1;
        }
        case Curve::EaseOutBounce: {
            if (t < 1 / 2.75f) {
                return 7.5625f * t * t;
            }
            else if (t < 2 / 2.75f) {
                t -= 1.5f / 2.75f;
                return 7.5625f * t * t + 0.75f;
            }
            else if (t < 2.5 / 2.75f) {
                t -= 2.25f / 2.75f;
                return 7.5625f * t * t + 0.9375f;
            }
            else {
                t -= 2.625f / 2.75f;
                return 7.5625f * t * t + 0.984375f;
            }
        }
        default: return t;
        }
    }


    class ITween {
    public:
        virtual ~ITween() = default;
        virtual void Update(float deltaTime) = 0;
        virtual bool IsPlaying() const = 0;
        virtual bool IsPaused() const = 0;
        virtual void Start() = 0;
        virtual void Stop() = 0;
        virtual void Pause() = 0;
        virtual void Resume() = 0;

        virtual const ::std::type_info& GetTypeInfo() const = 0;
        virtual void* GetTargetPtr() const = 0;
    };

    template<typename T>
    class Tween : public ITween
    {
    public:
        Tween(T* target, const T& endValue, double duration, Curve curve = Curve::Linear)
            : target(target), endValue(endValue), duration(duration), curve(curve) {
            if (target) {
                startValue = *target;
            }
            passedTime = 0;
        }

        void Start() {
            if (!target) return;

            startValue = *target;
            isPlaying = true;
            isPaused = false;
        }

        void Stop() {
            if (!target) return;

            isPlaying = false;
            isPaused = false;
            *target = startValue;
        }

        void Pause() {
            if (!isPlaying || isPaused) return;

            isPaused = true;
            isPlaying = false;
        }

        void Resume() {
            if (!isPaused) return;

            isPlaying = true;
            isPaused = false;
        }

        void Update(float deltaTime) {
            if (!isPlaying || !target) return;

            passedTime += deltaTime;

            float t = min(1.0f, passedTime / duration);
            *target = interpolate(t);

            if (t >= 1.0f) {
                isPlaying = false;
                *target = endValue;
            }
        }

        bool IsPlaying() const { return isPlaying; }
        bool IsPaused() const { return isPaused; }

        T GetCurrentValue() const { return target ? *target : T{}; }
        T* GetTarget() const { return target; }

        const std::type_info& GetTypeInfo() const override {
            return typeid(T);
        }

        void* GetTargetPtr() const override {
            return static_cast<void*>(target);
        }

    private:
        T* target;
        T startValue;
        T endValue;

        double duration;  // в секундах
        double passedTime;

        Curve curve;

        bool isPlaying = false;
        bool isPaused = false;

    private:
        T interpolate(float t) const {
            float easedT = EaseCurve(curve, t);
            return startValue + (endValue - startValue) * easedT;
        }
    };


    static ::std::vector<::std::unique_ptr<ITween>> _activeTweens;


    template<typename T, typename U>
    Tween<T>& CreateTween(T& target, const U& endValue, double duration, Curve curve = Curve::Linear) {

        // Searching for existing tween with this target
        auto it = ::std::find_if(_activeTweens.begin(), _activeTweens.end(),
            [&target](const ::std::unique_ptr<ITween>& tween) {
                if (tween->GetTypeInfo() != typeid(T)) {
                    return false;
                }
                return tween->GetTargetPtr() == static_cast<void*>(&target);
            });

        if (it != _activeTweens.end()) {
            _activeTweens.erase(it);
        }

        auto tween = ::std::make_unique<Tween<T>>(&target, static_cast<T>(endValue), duration, curve);
        Tween<T>& ref = *tween;
        _activeTweens.push_back(::std::move(tween));
        return ref;
    }

    template<typename T, typename U>
    Tween<T>& Animate(T& target, const U& endValue, double duration, Curve curve = Curve::Linear) {
        auto& tween = CreateTween(target, endValue, duration, curve);
        tween.Start();
        return tween;
    }

    inline void UpdateTweens(float deltaTime) {
        for (auto& tween : _activeTweens) {
            tween->Update(deltaTime);
        }

        _activeTweens.erase(
            ::std::remove_if(_activeTweens.begin(), _activeTweens.end(),
                [](const auto& tween) { return !tween->IsPlaying() && !tween->IsPaused(); }),
            _activeTweens.end()
        );
    }

    inline void ClearAllTweens() {
        _activeTweens.clear();
    }
}