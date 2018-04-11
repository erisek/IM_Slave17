
// CONFIG register renames.

// Oscillator options.
// The dSPIN needs to know what the clock frequency is because it uses that for some
//  calculations during operation.
#define CONFIG_OSC_SEL                 0x000F // Mask for this bit field.
#define CONFIG_INT_16MHZ               0x0000 // Internal 16MHz, no output
#define CONFIG_INT_16MHZ_OSCOUT_2MHZ   0x0008 // Default; internal 16MHz, 2MHz output
#define CONFIG_INT_16MHZ_OSCOUT_4MHZ   0x0009 // Internal 16MHz, 4MHz output
#define CONFIG_INT_16MHZ_OSCOUT_8MHZ   0x000A // Internal 16MHz, 8MHz output
#define CONFIG_INT_16MHZ_OSCOUT_16MHZ  0x000B // Internal 16MHz, 16MHz output
#define CONFIG_EXT_8MHZ_XTAL_DRIVE     0x0004 // External 8MHz crystal
#define CONFIG_EXT_16MHZ_XTAL_DRIVE    0x0005 // External 16MHz crystal
#define CONFIG_EXT_24MHZ_XTAL_DRIVE    0x0006 // External 24MHz crystal
#define CONFIG_EXT_32MHZ_XTAL_DRIVE    0x0007 // External 32MHz crystal
#define CONFIG_EXT_8MHZ_OSCOUT_INVERT  0x000C // External 8MHz crystal, output inverted
#define CONFIG_EXT_16MHZ_OSCOUT_INVERT 0x000D // External 16MHz crystal, output inverted
#define CONFIG_EXT_24MHZ_OSCOUT_INVERT 0x000E // External 24MHz crystal, output inverted
#define CONFIG_EXT_32MHZ_OSCOUT_INVERT 0x000F // External 32MHz crystal, output inverted

// Configure the functionality of the external switch input
#define CONFIG_SW_MODE                 0x0010 // Mask for this bit.
#define CONFIG_SW_HARD_STOP            0x0000 // Default; hard stop motor on switch.
#define CONFIG_SW_USER                 0x0010 // Tie to the GoUntil and ReleaseSW
//  commands to provide jog function.
//  See page 25 of datasheet.

// Configure the motor voltage compensation mode (see page 34 of datasheet)
#define CONFIG_EN_VSCOMP               0x0020  // Mask for this bit.
#define CONFIG_VS_COMP_DISABLE         0x0000  // Disable motor voltage compensation.
#define CONFIG_VS_COMP_ENABLE          0x0020  // Enable motor voltage compensation.

// Configure overcurrent detection event handling
#define CONFIG_OC_SD                   0x0080  // Mask for this bit.
#define CONFIG_OC_SD_DISABLE           0x0000  // Bridges do NOT shutdown on OC detect
#define CONFIG_OC_SD_ENABLE            0x0080  // Bridges shutdown on OC detect

// Configure the slew rate of the power bridge output
#define CONFIG_POW_SR                  0x0300  // Mask for this bit field.
#define CONFIG_SR_180V_us              0x0000  // 180V/us
#define CONFIG_SR_290V_us              0x0200  // 290V/us
#define CONFIG_SR_530V_us              0x0300  // 530V/us

// Integer divisors for PWM sinewave generation
//  See page 32 of the datasheet for more information on this.
#define CONFIG_F_PWM_DEC               0x1C00      // mask for this bit field
#define CONFIG_PWM_MUL_0_625           (0x00)<<10
#define CONFIG_PWM_MUL_0_75            (0x01)<<10
#define CONFIG_PWM_MUL_0_875           (0x02)<<10
#define CONFIG_PWM_MUL_1               (0x03)<<10
#define CONFIG_PWM_MUL_1_25            (0x04)<<10
#define CONFIG_PWM_MUL_1_5             (0x05)<<10
#define CONFIG_PWM_MUL_1_75            (0x06)<<10
#define CONFIG_PWM_MUL_2               (0x07)<<10

// Multiplier for the PWM sinewave frequency
#define CONFIG_F_PWM_INT               0xE000     // mask for this bit field.
#define CONFIG_PWM_DIV_1               (0x00)<<13
#define CONFIG_PWM_DIV_2               (0x01)<<13
#define CONFIG_PWM_DIV_3               (0x02)<<13
#define CONFIG_PWM_DIV_4               (0x03)<<13
#define CONFIG_PWM_DIV_5               (0x04)<<13
#define CONFIG_PWM_DIV_6               (0x05)<<13
#define CONFIG_PWM_DIV_7               (0x06)<<13

void L6470_transfer_all(int *add, int bytes, unsigned long *val, bool noWaitBusy = false);
void SPI_SendAll(int *data, bool noWaitBusy = false);

void L6470_transfer_index(int index, int add, int bytes, long val, bool noWaitBusy = false);
void SPI_SendTo(int index, uint8_t data, bool noWaitBusy = false);

void L6470_resetAll()
{
	//  0x00を一通り送る
	for(int i=0;i<3;i++) {
		L6470_sendNull();
	}

	//  resetDevice
	L6470_resetDevice_allMotor();
}

void L6470_resetWithResetPin() {
	digitalWrite(RESET, LOW);
	delay(100);
	digitalWrite(RESET, HIGH);
	delay(100);
}

void waitBusy() {
	while (!digitalRead(BUSY)) {};
}

// チェインされたすべてのデバイスに同じ値を設定する
void SPI_Broadcast(uint8_t data)
{
	waitBusy();

	digitalWrite(SS, LOW);
	for (int8_t i = 0 ; i < MOTOR_NUM; i++)
	{
		SPI.transfer(data);
	}
	digitalWrite(SS, HIGH);
}

void SPI_SendTo(int index, uint8_t data, bool noWaitBusy)
{
	if(!noWaitBusy) waitBusy();

	digitalWrite(SS, LOW);
	for (int8_t i = 0 ; i < MOTOR_NUM; i++)
	{
		if ((1 << i)&index)
			SPI.transfer(data);
		else
			SPI.transfer(0x00);
	}
	digitalWrite(SS, HIGH);
}

void L6470_transfer(int add, int bytes, long val)
{
	int data[3];
	SPI_Broadcast(add);
	for (int i = 0; i <= bytes - 1; i++)
	{
		data[i] = val & 0xff;
		val = val >> 8;
	}
	if (bytes == 3)
	{
		SPI_Broadcast(data[2]);
	}
	if (bytes >= 2) {
		SPI_Broadcast(data[1]);
	}
	if (bytes >= 1) {
		SPI_Broadcast(data[0]);
	}
}

void L6470_transfer_index(int index, int add, int bytes, long val, bool noWaitBusy)
{
	int data[3];
	SPI_SendTo(index, add, noWaitBusy);
	for (int i = 0; i <= bytes - 1; i++)
	{
		data[i] = val & 0xff;
		val = val >> 8;
	}
	if (bytes == 3)
	{
		SPI_SendTo(index, data[2], noWaitBusy);
	}
	if (bytes >= 2) {
		SPI_SendTo(index, data[1], noWaitBusy);
	}
	if (bytes >= 1) {
		SPI_SendTo(index, data[0], noWaitBusy);
	}
}

void L6470_run(int dia, unsigned long spd) {
	if (dia == 1)
		L6470_transfer(0x51, 3, spd);
	else
		L6470_transfer(0x50, 3, spd);
}

void L6470_run_index(int index, int dia, unsigned long spd){
	if (dia == 1)
		L6470_transfer_index(index, 0x51, 3, spd);
	else
		L6470_transfer_index(index, 0x50, 3, spd);
}

void L6470_stepclock(int dia) {
	if (dia == 1)
		SPI_Broadcast(0x59);
	else
		SPI_Broadcast(0x58);
}
void L6470_move(int dia, unsigned long n_step) {
	if (dia == 1)
		L6470_transfer(0x41, 3, n_step);
	else
		L6470_transfer(0x40, 3, n_step);
}
void L6470_move_index(int index, int dia, unsigned long n_step)
{
	if (dia == 1)
		L6470_transfer_index(index, 0x41, 3, n_step);
	else
		L6470_transfer_index(index, 0x40, 3, n_step);
}

void L6470_goto(unsigned long pos)
{
	L6470_transfer(0x60, 3, pos);
}

void L6470_goto_index(int index, unsigned long pos)
{
	L6470_transfer_index(index, 0x60, 3, pos);
}

void L6470_goto_index_noWait(int index, unsigned long pos)
{
	L6470_transfer_index(index, 0x60, 3, pos, true);
}


void L6470_gotodia(int dia, int pos) {
	if (dia == 1)
		L6470_transfer(0x69, 3, pos);
	else
		L6470_transfer(0x68, 3, pos);
}
void L6470_gotodia_index(int index, int dia, int pos) {
	if (dia == 1)
		L6470_transfer_index(index, 0x69, 3, pos);
	else
		L6470_transfer_index(index, 0x68, 3, pos);
}

void L6470_gountil(int act, int dia, unsigned long spd) {
	if (act == 1)
		if (dia == 1)
			L6470_transfer(0x8b, 3, spd);
		else
			L6470_transfer(0x8a, 3, spd);
	else if (dia == 1)
		L6470_transfer(0x83, 3, spd);
	else
		L6470_transfer(0x82, 3, spd);
}

void L6470_gountil_index(int index, int act, int dia, unsigned long spd) {
	if (act == 1)
		if (dia == 1)
			L6470_transfer_index(index, 0x8b, 3, spd);
		else
			L6470_transfer_index(index, 0x8a, 3, spd);
	else if (dia == 1)
		L6470_transfer_index(index, 0x83, 3, spd);
	else
		L6470_transfer_index(index, 0x82, 3, spd);
}
void L6470_relesesw(int act, int dia) {
	if (act == 1)
		if (dia == 1)
			SPI_Broadcast(0x9b);
		else
			SPI_Broadcast(0x9a);
	else if (dia == 1)
		SPI_Broadcast(0x93);
	else
		SPI_Broadcast(0x92);
}

void L6470_relesesw_index(int index, int act, int dia) {
	if (act == 1)
		if (dia == 1)
			SPI_SendTo(index, 0x9b);
		else
			SPI_SendTo(index, 0x9a);
	else if (dia == 1)
		SPI_SendTo(index, 0x93);
	else
		SPI_SendTo(index, 0x92);
}

void L6470_gohome() {
	SPI_Broadcast(0x70);
}
void L6470_gomark() {
	SPI_Broadcast(0x78);
}
void L6470_resetpos() {
	SPI_Broadcast(0xd8);
}
void L6470_resetpos_index(int index) {
	SPI_SendTo(index, 0xd8);
}
void L6470_softstop() {
	SPI_Broadcast(0xb0);
}
void L6470_softstop_index(int index) {
	SPI_SendTo(index, 0xb0);
}

void L6470_hardstop() {
	SPI_Broadcast(0xb8);
}

void L6470_hardstop_index(int index)
{
	SPI_SendTo(index, 0xb8);
}

void L6470_softhiz() {
	SPI_Broadcast(0xa0);
}

void L6470_hardhiz() {
	SPI_Broadcast(0xa8);
}

void L6470_busydelay(long time) { //BESYが解除されるまで待機
	waitBusy();
	delay(time);
}

void L6470_getStatus() {
	SPI_Broadcast(0xd0);
	SPI_Broadcast(0x00);
	SPI_Broadcast(0x00);
}

//////////////////////////////////////////////////
//  個別に指定するメソッド
//////////////////////////////////////////////////

void L6470_run_all(int *dia, unsigned long *spd) {
	L6470_run_all_opt(dia, spd, false);
}

void L6470_run_all_noWait(int *dia, unsigned long *spd) {
	L6470_run_all_opt(dia, spd, true);
}

void L6470_run_all_opt(int *dia, unsigned long *spd, bool noWaitBusy) {
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		if(i == 0)
			addrs[i] = dia[i] == 1 ? 0x51 : 0x50;
		else if(i == 1)
			addrs[i] = dia[i] == 0 ? 0x51 : 0x50;
	}

	L6470_transfer_all(addrs, 3, spd, noWaitBusy);
}

void L6470_move_all(int *dia, unsigned long *n_step)
{
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		addrs[i] = dia[i] == 1 ? 0x41 : dia[i] == 0 ? 0x40 : 0x00;
	}

	L6470_transfer_all(addrs, 3, n_step);
}

void L6470_goto_all(unsigned long *positions)
{
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		addrs[i] = 0x60;
	}

	L6470_transfer_all(addrs, 3, positions);
}

void L6470_hardstop_all(bool *stop)
{
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		addrs[i] = stop[i] ? 0xb8 : 0x00;
	}
	SPI_SendAll(addrs);
}

void L6470_softstop_all(bool *stop) {
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		addrs[i] = stop[i] ? 0xb0 : 0x00;
	}
	SPI_SendAll(addrs);
}

void L6470_resetDevice_allMotor() {
	int addrs[MOTOR_NUM];

	for (int i = 0; i < MOTOR_NUM; i++)
	{
		addrs[i] = 0xc0;
	}
	SPI_SendAll(addrs);
}

void L6470_sendNull() {
	waitBusy();
	for (int8_t i = 0 ; i < MOTOR_NUM; i++)
	{
		SPI.transfer(0);
	}
}

//////

void L6470_transfer_all(int *add, int bytes, unsigned long *val, bool noWaitBusy)
{
	unsigned long values[MOTOR_NUM];
	int data[3][MOTOR_NUM];

	SPI_SendAll(add, noWaitBusy);

	for (int j = 0; j < MOTOR_NUM; j++)
	{
		values[j] = val[j];

		for (int i = 0; i <= bytes - 1; i++)
		{
			data[i][j] = values[j] & 0xff;
			values[j] >>= 8;
		}
	}

	//
	if (bytes == 3)
	{
		SPI_SendAll(data[2], noWaitBusy);
	}
	if (bytes >= 2)
	{
		SPI_SendAll(data[1], noWaitBusy);
	}
	if (bytes >= 1)
	{
		SPI_SendAll(data[0], noWaitBusy);
	}
}

void SPI_SendAll(int *data, bool noWaitBusy)
{
	if(!noWaitBusy)
		waitBusy();

	digitalWrite(SS, LOW);
	for (int8_t i = 0 ; i < MOTOR_NUM; i++)
	{
		SPI.transfer(data[i]);
	}
	digitalWrite(SS, HIGH);
}

void L6470_setparam_abspos(long val) {L6470_transfer(0x01, 3, val);}
void L6470_setparam_elpos(long val) {  L6470_transfer(0x02, 2, val);}
void L6470_setparam_mark(long val) {  L6470_transfer(0x03, 3, val);}
void L6470_setparam_acc(long val) {  L6470_transfer(0x05, 2, val);}
void L6470_setparam_dec(long val) {  L6470_transfer(0x06, 2, val);}
void L6470_setparam_maxspeed(long val) {  L6470_transfer(0x07, 2, val);}
void L6470_setparam_minspeed(long val) {  L6470_transfer(0x08, 2, val);}
void L6470_setparam_fsspd(long val) {  L6470_transfer(0x15, 2, val);}
void L6470_setparam_kvalhold(long val) {  L6470_transfer(0x09, 1, val);}
void L6470_setparam_kvalrun(long val) {  L6470_transfer(0x0a, 1, val);}
void L6470_setparam_kvalacc(long val) {  L6470_transfer(0x0b, 1, val);}
void L6470_setparam_kvaldec(long val) {  L6470_transfer(0x0c, 1, val);}
void L6470_setparam_intspd(long val) {  L6470_transfer(0x0d, 2, val);}
void L6470_setparam_stslp(long val) {  L6470_transfer(0x0e, 1, val);}
void L6470_setparam_fnslpacc(long val) {  L6470_transfer(0x0f, 1, val);}
void L6470_setparam_fnslpdec(long val) {  L6470_transfer(0x10, 1, val);}
void L6470_setparam_ktherm(long val) {  L6470_transfer(0x11, 1, val);}
void L6470_setparam_ocdth(long val) {  L6470_transfer(0x13, 1, val);}
void L6470_setparam_stallth(long val) {  L6470_transfer(0x14, 1, val);}
void L6470_setparam_stepmood(long val) {  L6470_transfer(0x16, 1, val);}
void L6470_setparam_alarmen(long val) {  L6470_transfer(0x17, 1, val);}
void L6470_setparam_config(long val) {  L6470_transfer(0x18, 2, val);}

void L6470_setparam_abspos_index(int idx, long val) {L6470_transfer_index(idx, 0x01, 3, val);}
void L6470_setparam_kvalhold_index(int idx, long val) {  L6470_transfer_index(idx, 0x09, 1, val);}
void L6470_setparam_kvalrun_index(int idx, long val) {  L6470_transfer_index(idx, 0x0a, 1, val);}
void L6470_setparam_kvalacc_index(int idx, long val) {  L6470_transfer_index(idx, 0x0b, 1, val);}
void L6470_setparam_kvaldec_index(int idx, long val) {  L6470_transfer_index(idx, 0x0c, 1, val);}
void L6470_setparam_stepmood_index(int idx, long val) {  L6470_transfer_index(idx, 0x16, 1, val);}
void L6470_setparam_status_index(int idx, long val) {  L6470_transfer_index(idx, 0x19, 2, val);}
void L6470_setparam_config_index(int idx, long val){L6470_transfer_index(idx, 0x18,2,val);}

void L6470_setparam_acc_index(int idx, long val) {  L6470_transfer_index(idx, 0x05, 2, val);}
void L6470_setparam_dec_index(int idx, long val) {  L6470_transfer_index(idx, 0x06, 2, val);}
void L6470_setparam_maxspeed_index(int idx, long val) {  L6470_transfer_index(idx, 0x07, 2, val);}
void L6470_setparam_minspeed_index(int idx, long val) {  L6470_transfer_index(idx, 0x08, 2, val);}
void L6470_setparam_fsspd_index(int idx, long val) {  L6470_transfer_index(idx, 0x15, 2, val);}

void L6470_setparam_stallth_index(int idx, long val) {  L6470_transfer_index(idx, 0x14, 1, val);}

void L6470_setparam_intspd_index(int idx, long val) {  L6470_transfer_index(idx, 0x0d, 2, val);}
void L6470_setparam_stslp_index(int idx, long val) {  L6470_transfer_index(idx, 0x0e, 1, val);}
void L6470_setparam_fnslpacc_index(int idx, long val) {  L6470_transfer_index(idx, 0x0f, 1, val);}
void L6470_setparam_fnslpdec_index(int idx, long val) {  L6470_transfer_index(idx, 0x10, 1, val);}
void L6470_setparam_ocdth_index(int idx, long val) {  L6470_transfer_index(idx, 0x13, 1, val);}
