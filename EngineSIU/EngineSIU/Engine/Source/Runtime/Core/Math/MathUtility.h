#pragma once
#include <cmath>
#include <numbers>
#include "Core/HAL/PlatformType.h"
#include <cstdlib>
#define PI                   (3.1415926535897932f)
#define SMALL_NUMBER         (1.e-8f)
#define KINDA_SMALL_NUMBER   (1.e-4f)

#define PI_DOUBLE            (3.141592653589793238462643383279502884197169399)


struct FMath
{
	/** A와 B중에 더 작은 값을 반환합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Min(const T A, const T B)
	{
		return A < B ? A : B;
	}

	/** A와 B중에 더 큰 값을 반환합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Max(const T A, const T B)
	{
		return B < A ? A : B;
	}

    /** A, B, C 중에 가장 큰 값을 반환합니다. */
    template <typename T>
    [[nodiscard]] static FORCEINLINE constexpr T Max3(const T A, const T B, const T C)
    {
        return Max(A, Max(B, C));
    }

	/** X를 Min과 Max의 사이의 값으로 클램핑 합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Clamp(const T X, const T MinValue, const T MaxValue)
	{
		return Max(Min(X, MaxValue), MinValue);
	}

	/** A의 절댓값을 구합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Abs(const T A)
	{
		return A < T(0) ? -A : A;
	}

    /** Returns 1, 0, or -1 depending on relation of T to 0 */
    template< class T > 
    static constexpr FORCEINLINE T Sign( const T A )
	{
        return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** A의 제곱을 구합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Pow(const T A)
	{
		return A * A;
	}

	// A의 제곱근을 구합니다.
	[[nodiscard]] static FORCEINLINE float Sqrt(float A) { return sqrtf(A); }
	[[nodiscard]] static FORCEINLINE double Sqrt(double A) { return sqrt(A); }

	/** A의 역제곱근을 구합니다. */
	[[nodiscard]] static FORCEINLINE float InvSqrt(float A) { return 1.0f / sqrtf(A); }
	[[nodiscard]] static FORCEINLINE double InvSqrt(double A) { return 1.0 / sqrt(A); }

	/** A와 B를 Alpha값에 따라 선형으로 보간합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Lerp(const T& A, const T& B, float Alpha)
	{
		return static_cast<T>((A * (1.0f - Alpha)) + (B * Alpha));
	}

	/** A와 B를 Alpha값에 따라 선형으로 보간합니다. */
	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr T Lerp(const T& A, const T& B, double Alpha)
	{
		return static_cast<T>((A * (1.0 - Alpha)) + (B * Alpha));
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr auto RadiansToDegrees(const T& RadVal) -> decltype(RadVal * (180.0f / PI))
	{
		return RadVal * (180.0f / PI);
	}

	[[nodiscard]] static FORCEINLINE constexpr float RadiansToDegrees(float RadVal)
	{
		return RadVal * (180.0f / PI);
	}

	[[nodiscard]] static FORCEINLINE constexpr double RadiansToDegrees(double RadVal)
	{
		return RadVal * (180.0 / PI_DOUBLE);
	}

	template <typename T>
	[[nodiscard]] static FORCEINLINE constexpr auto DegreesToRadians(const T& DegVal) -> decltype(DegVal * (PI / 180.0f))
	{
		return DegVal * (PI / 180.0f);
	}

	[[nodiscard]] static FORCEINLINE constexpr float DegreesToRadians(float DegVal)
	{
		return DegVal * (PI / 180.0f);
	}

	[[nodiscard]] static FORCEINLINE constexpr double DegreesToRadians(double DegVal)
	{
		return DegVal * (PI_DOUBLE / 180.0);
	}

    // Returns e^Value
    static FORCEINLINE float Exp( float Value ) { return expf(Value); }
    static FORCEINLINE double Exp(double Value) { return exp(Value); }

    // Returns 2^Value
    static FORCEINLINE float Exp2( float Value ) { return powf(2.f, Value); /*exp2f(Value);*/ }
    static FORCEINLINE double Exp2(double Value) { return pow(2.0, Value); /*exp2(Value);*/ }

    static FORCEINLINE float Loge( float Value ) {	return logf(Value); }
    static FORCEINLINE double Loge(double Value) { return log(Value); }

    static FORCEINLINE float LogX( float Base, float Value ) { return Loge(Value) / Loge(Base); }
    static FORCEINLINE double LogX(double Base, double Value) { return Loge(Value) / Loge(Base); }

    // 1.0 / Loge(2) = 1.4426950f
    static FORCEINLINE float Log2( float Value ) { return Loge(Value) * std::numbers::log2e_v<float>; }	
    // 1.0 / Loge(2) = 1.442695040888963387
    static FORCEINLINE double Log2(double Value) { return Loge(Value) * std::numbers::log2e; }


	[[nodiscard]] static FORCEINLINE double Cos(double RadVal) { return cos(RadVal); }
	[[nodiscard]] static FORCEINLINE float Cos(float RadVal) { return cosf(RadVal); }

	[[nodiscard]] static FORCEINLINE double Sin(double RadVal) { return sin(RadVal); }
	[[nodiscard]] static FORCEINLINE float Sin(float RadVal) { return sinf(RadVal); }

	[[nodiscard]] static FORCEINLINE double Tan(double RadVal) { return tan(RadVal); }
	[[nodiscard]] static FORCEINLINE float Tan(float RadVal) { return tanf(RadVal); }

	[[nodiscard]] static FORCEINLINE double Acos(double Value) { return acos(Value); }
	[[nodiscard]] static FORCEINLINE float Acos(float Value) { return acosf(Value); }

	[[nodiscard]] static FORCEINLINE double Asin(double Value) { return asin(Value); }
	[[nodiscard]] static FORCEINLINE float Asin(float Value) { return asinf(Value); }

	[[nodiscard]] static FORCEINLINE double Atan(double Value) { return atan(Value); }
	[[nodiscard]] static FORCEINLINE float Atan(float Value) { return atanf(Value); }

	[[nodiscard]] static FORCEINLINE double Atan2(double Y, double X) { return atan2(Y, X); }
	[[nodiscard]] static FORCEINLINE float Atan2(float Y, float X) { return atan2f(Y, X); }

	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float Value)
	{
		*ScalarSin = sinf(Value);
		*ScalarCos = cosf(Value);
	}

	static FORCEINLINE void SinCos(double* ScalarSin, double* ScalarCos, double Value)
	{
		*ScalarSin = sin(Value);
		*ScalarCos = cos(Value);
	}

    template <typename T>
	[[nodiscard]] static FORCEINLINE T Square(T Value) { return Value * Value; }


	[[nodiscard]] static FORCEINLINE int32 CeilToInt(float Value) { return static_cast<int32>(ceilf(Value)); }
	[[nodiscard]] static FORCEINLINE int32 CeilToInt(double Value) { return static_cast<int32>(ceil(Value)); }

    template <typename T>
    [[nodiscard]] static FORCEINLINE int32 CeilToInt(T Value) { return static_cast<int32>(ceil(Value)); }


	[[nodiscard]] static FORCEINLINE float UnwindDegrees(float A)
	{
		while (A > 180.0f)
		{
			A -= 360.0f;
		}
		while (A < -180.0f)
		{
			A += 360.0f;
		}
		return A;
	}

    /** 0.0f 이상 1.0f 이하 범위의 난수(균등 분포)를 반환 */
    [[nodiscard]] static FORCEINLINE float FRand()
    {
        return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }

    /**
     * InMin 이상 InMax 이하 범위의 난수(균등 분포)를 반환
     *
     * @param InMin 난수 최소값
     * @param InMax 난수 최대값
     */
    [[nodiscard]] static FORCEINLINE float FRandRange(float InMin, float InMax)
    {
        return InMin + FRand() * (InMax - InMin);
    }

    // 페를린 Noise 1D 관련
private:
    static constexpr uint8 Permutation[256] = {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,
        140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
        247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,
        57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
        74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,
        60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,
        65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,
        200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,
        52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,
        207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
        119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
        129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,
        218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,
        81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,
        184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,
        222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
    };

    [[nodiscard]] static FORCEINLINE float Fade(float t)
    {
        // 6t^5 - 15t^4 + 10t^3
        return t * t * t * (t * (t * 6.f - 15.f) + 10.f);
    }

    [[nodiscard]] static FORCEINLINE float Gradient(int hash, float x)
    {
        int h = hash & 15;
        float grad = 1.f + static_cast<float>(h & 7); // grad ∈ [1,8]
        return (h & 8) ? -grad * x : grad * x;
    }

public:
    /** Perlin 노이즈 (1D) */
    [[nodiscard]] static FORCEINLINE float PerlinNoise1D(float x)
    {
        int xi = static_cast<int>(floorf(x)) & 255;
        float xf = x - floorf(x);
        float u = Fade(xf);

        float g1 = Gradient(Permutation[xi], xf);
        float g2 = Gradient(Permutation[(xi + 1) & 255], xf - 1.f);

        return Lerp(g1, g2, u); // 출력 범위는 대략 -1.0 ~ 1.0
    }

    
};
