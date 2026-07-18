/*
 * i2c_rtc.c
 *
 * Created: 2026-07-01 오후 3:37:51
 *  Author: kccistc
 */ 
#include "i2c.h"


void i2c_init(void)
{
	DS1307_DDR |= 1 << DS1307_SCL | 1 << DS1307_SDA;		// 출력 설정
	TWSR=0x00;   
	
	TWBR = TWBR_VALUE;					// I2C 클록 주파수 설정 100KHz
}

uint8_t i2c_start(void)
{
	uint8_t st;
	
	TWCR = 1 << TWINT | 1 << TWSTA | 1 << TWEN;
	while (!(TWCR & (1 << TWINT)));
	st = TWSR & 0xf8;
	
	return (st == TWSR_START || st == TWSR_REP_START ) ? 0 : st;
}

uint8_t i2c_slave_addr_send(uint8_t addr_rw)
{
	TWDR = addr_rw;
	TWCR = 1 << TWINT | 1 << TWEN;
	while( !(TWCR & (1 << TWINT)));
	
	return (TWSR & 0xf8);
}

uint8_t i2c_data_write(uint8_t data)
{	
	TWDR = data;
	TWCR = 1 << TWINT | 1 << TWEN;
	while( !(TWCR & (1 << TWINT)));
	
	return (TWSR & 0xf8);    // 7~3bit까지 값을 취함
}

void i2c_stop(void)
{
	TWCR = 1 << TWINT | 1 << TWSTO | 1 << TWEN;
	_delay_us(10);
}

uint8_t i2c_data_read_acksend(void)
{
	
	TWCR = 1 << TWINT | 1 << TWEN | 1 << TWEA;    // TWEA=1 ACK자동 전송
	while( !(TWCR & (1 << TWINT)));
	
	return (TWDR);
}

uint8_t i2c_data_read_nacksend(void)
{
	
	TWCR = 1 << TWINT | 1 << TWEN ;    // TWEA=0 NACK자동 전송
	while( !(TWCR & (1 << TWINT)));
	
	return (TWDR);
}

uint8_t bcd_to_dec(uint8_t bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t dec_to_bcd(uint8_t dec)
{
	return (((dec / 10) << 4 ) | (dec % 10));
}

void init_date_time(t_ds1307* ds1307)
{
	ds1307->year=26;
	ds1307->month=07;
	ds1307->date=1;
	ds1307->dayofweek=2;  // Fri
	ds1307->hours=16;
	ds1307->minutes=45;
	ds1307->seconds=00;
}

void init_ds1307(t_ds1307* ds1307)
{
	uint8_t status, data;
	uint8_t* p = (uint8_t*)ds1307;
	
	status = i2c_start();
	if (status)
	{
		return;
	}
	status = i2c_slave_addr_send((SLAVE_ADDR << 1) | 0);   // 0xC0 : SLAVE_ADDR+W
	if (status != TWSR_MT_SLA_ACK) {
		i2c_stop();
		return;
	}
	status = i2c_data_write(0x00);   // 0xC0 : SLAVE_ADDR+W
	if (status != TWSR_MT_DATA_ACK) {
		i2c_stop();
		return;
	}
	
	for (int i = 0; i < 7; i++)
	{
		data = dec_to_bcd(p[i]);
		
		if (i == 0)
		{
			data &= 0x7f;
		}
		
		status =i2c_data_write(data);
		if (status != TWSR_MT_DATA_ACK) {
			i2c_stop();
			return;
		}
	}
	
	i2c_stop();
	
	_delay_ms(5);
}

void read_ds1307(t_ds1307* ds1307)
{
	uint8_t status;
	uint8_t* p = (uint8_t*)ds1307;
	
	status = i2c_start();
	if (status) {
		return;
	}
	status = i2c_slave_addr_send((SLAVE_ADDR << 1) | 0);   // 0xC0 : SLAVE_ADDR+W
	if (status != TWSR_MT_SLA_ACK) {
		i2c_stop();
		return;
	}
	status = i2c_data_write(0x00);   // 0xC0 : SLAVE_ADDR+W
	if (status != TWSR_MT_DATA_ACK) {
		i2c_stop();
		return;
	}
	
	i2c_stop();
	
	status = i2c_start();
	if (status) {
		return;
	}
	status = i2c_slave_addr_send((SLAVE_ADDR << 1) + 1);   // 0xC0 : SLAVE_ADDR+R
	if (status != TWSR_MR_SLA_ACK) {
		i2c_stop();
		return;
	}
	
	for (int i = 0; i < 6; i++)
	{
		p[i] = bcd_to_dec(i2c_data_read_acksend());
	}
	p[6] = bcd_to_dec(i2c_data_read_nacksend());
	
	i2c_stop();
}

uint8_t is_equal(t_ds1307* old_data, t_ds1307* new_data)
{
	if (old_data->seconds   != new_data->seconds)   return 0;
	if (old_data->minutes   != new_data->minutes)   return 0;
	if (old_data->hours     != new_data->hours)     return 0;
	if (old_data->dayofweek != new_data->dayofweek) return 0;
	if (old_data->date      != new_data->date)      return 0;
	if (old_data->month     != new_data->month)     return 0;
	if (old_data->year      != new_data->year)      return 0;

	return 1;
}
