/*
 * i2c_rtc.h
 *
 * Created: 2026-07-01 오후 3:58:02
 *  Author: kccistc
 */ 
#ifndef I2C_H_
#define I2C_H_

#define F_CPU 16000000UL  //16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <stdio.h>
#include <string.h>

#define DS1307_DDR  DDRD
#define DS1307_PORT PORTD

#define DS1307_SCL    0
#define DS1307_SDA    1

#define SLAVE_ADDR 0x68		// Tiny RTC I2C 주소 
#define I2C_FREQUENCY 100000UL
#define TWBR_VALUE ((F_CPU / I2C_FREQUENCY - 16) / 2)

//--------- TWI 상태 코드 -------------
// TWSR(TWI Status Register) : 7번~3번 bit(5bit를 참조) : I2c통신의 전송 상태나 오류를 나타 낸다 
#define TWSR_START  0x08  // START 조건 전송 완료 
#define TWSR_REP_START 0x10   // TWSR_REP_START (0x10) - Repeated START
#define TWSR_MT_SLA_ACK  0x18  // MASTER가 SLAVE ADDR + W 전송후 ACK 수신 
#define TWSR_MT_DATA_ACK  0x28  // MASTER가 SLAVE DATA 전송후 ACK 수신 
#define TWSR_MR_SLA_ACK  0x40   // MASTER가 SLAVE ADDR + R 전송후 ACK 수신 
#define TWSR_MR_DATA_ACK  0x50   // MASTER가 DATA 수신뒤 + ACK 전송  
#define TWSR_MR_DATA_NACK  0x58   // MASTER가 DATA 수신뒤 + NACK 전송  (마지막)
/*
 ATmega128a TWI(I2C) 관련 register
 TWCR : TWI Control Register 
    -각 BIT의 역활 --
	 TWINT : 인터럽트 플래그 (1 이면 clear(일반 flag와 다르다)) I2C작업이 끝나면 set
	 TWEA : ACK enable (1 : 자동으로 ACK 가 전송 0: NACK 전송)
	 TWSTA : START 신호 발생 (MASTER  전용, SLVAE는 사용 안함)
	 TWSTO : STOP 신호 발생 (MASTER  전용, SLVAE는 사용 안함)
	 TWEN : TWI(I2C) Enable ( I2C 하드웨어를 활성화)
	 TWIE : Interrupt Enable (TWI(i2C) interrupt를 활성화) 
 TWSR : TWI Status Register (상위 5bit == 상태 코드, 하위 2bit: Prescaler)
 TWBR : TWI Bit Rate Register 
 TWDR : TWI Data Register 
 TWAR : TWI Address Register ( Slave 모드에서 사용 : 나의 어드레스를 설정 )
*/

void i2c_init(void);
uint8_t i2c_start(void);
uint8_t i2c_slave_addr_send(uint8_t addr_rw);
uint8_t i2c_data_write(uint8_t data);

void i2c_stop(void);
uint8_t i2c_data_read_acksend(void);
uint8_t i2c_data_read_nacksend(void);

uint8_t bcd_to_dec(uint8_t bcd);
uint8_t dec_to_bcd(uint8_t dec);

typedef struct _ds1307
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t dayofweek;   // 1: sun  2: mon
	uint8_t date;
	uint8_t month;
	uint8_t year;
	//uint8_t ampm;		 // 1: pm   0: am
	//uint8_t hourmode;    // 0:24    1:12
} t_ds1307;

void init_date_time(t_ds1307* ds1307);
void init_ds1307(t_ds1307* ds1307);
void read_ds1307(t_ds1307* ds1307);
uint8_t is_equal(t_ds1307* old_data, t_ds1307* new_data);

#endif