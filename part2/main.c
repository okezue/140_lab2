#include <stdint.h>
#define R(x) (*(volatile uint32_t *)(x))
#define GB 0x48000400
#define GC 0x48000800
#define EXTI 0x40010400
void EXTI15_10_IRQHandler(void)
{
	R(EXTI + 0x14) = 1u << 13;
	R(GB + 0x14) ^= 1u<<7;
}
int main(void)
{
	R(0x4002104C) |= 6;
	R(0x40021060) |= 1;
	R(GB) = (R(GB) & ~(3u << 14)) | (1u << 14);
	R(GC) &= ~(3u << 26);
	R(0x40010014) = (R(0x40010014) & ~(0xFu<<4)) | (2u << 4);
	R(EXTI+8) |= 1u << 13;
	R(EXTI) |= 1u << 13;
	R(0xE000E104) = 1u << 8;
	while (1)
	{
	};
}
