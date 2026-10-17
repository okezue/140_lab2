#include <stdint.h>
#define R(x) (*(volatile uint32_t *)(x))
#define GA 0x48000000
#define GB 0x48000400
#define GC 0x48000800
#define T4 0x40000800
static const uint32_t g[] = {GC, GB, GA};
static const int p[] = {7, 7, 9};
static int n;
static void cfg(uint32_t a, int k)
{
	R(a) = (R(a) & ~(3u << (2*k))) | (1u << (2 * k));
}
static void led(int i)
{
	for (int k = 0; k < 3; k++) R(g[k] + 0x18) = 1u << (p[k]+16);
	R(g[i] + 0x18) = 1u << p[i];
}
void TIM4_IRQHandler(void)
{
	R(T4 + 0x10) = 0;
	n = (n+1) % 3;
	led(n);
}
int main(void)
{
	R(0x4002104C) |= 7;
	R(0x40021058) |= 1u << 2;
	for (int k = 0; k<3; k++) cfg(g[k], p[k]);
	led(0);
	R(T4 + 0x28) = 3999;
	R(T4 + 0x2C) = 999;
	R(T4 + 0x14) = 1;
	R(T4 + 0x10) = 0;
	R(T4 + 0xC) = 1;
	R(0xE000E100) = 1u << 30;
	R(T4) |= 1;
	while (1)
	{
	};
}
