//////////////////////////////////////////////////////////////////////////////
//	モーターのパラメータを諸々設定
void SetupMotorParameters() {

	// // //	昇降モーターの設定 24Vversion
  // L6470_setparam_acc_index(MAKI_MOTOR_INDEX, 512); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  // L6470_setparam_dec_index(MAKI_MOTOR_INDEX, 512); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  // L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 55); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  // L6470_setparam_minspeed_index(MAKI_MOTOR_INDEX, 0x000); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  // L6470_setparam_fsspd_index(MAKI_MOTOR_INDEX, 0x3ff); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])
  //
  // L6470_setparam_intspd_index(MAKI_MOTOR_INDEX, 0x489);//0x1ED6
  // L6470_setparam_stslp_index(MAKI_MOTOR_INDEX, 0x9D);//0x20
  // L6470_setparam_fnslpacc_index(MAKI_MOTOR_INDEX, 0x162);//0x50
  // L6470_setparam_fnslpdec_index(MAKI_MOTOR_INDEX, 0x162);
	// L6470_setparam_stepmood_index(MAKI_MOTOR_INDEX, 0x07);	// マイクロステップ 1/128
  // L6470_setparam_ocdth_index(MAKI_MOTOR_INDEX, 0xF); //オーバーカレントの電流スレッショルド
  // L6470_setparam_stallth_index(MAKI_MOTOR_INDEX, 0x8);
  //
	// //	共通設定
  // L6470_setparam_alarmen(0x30);	//	アラームはストールだけ
	// L6470_setparam_config(CONFIG_VS_COMP_ENABLE | CONFIG_INT_16MHZ);	//	内蔵クロックで動作、VS-COMP使う
  //
  // //  電圧設定
  // int a = 0x35; //  aのモーターのトルク（arduinoから遠い方）ここも96から80とかに下げてみる
  // int b = 40; //  回転台のトルク
  //
  // L6470_setparam_kvalhold_index(MAKI_MOTOR_INDEX, a); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  // L6470_setparam_kvalrun_index(MAKI_MOTOR_INDEX, a); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  // L6470_setparam_kvalacc_index(MAKI_MOTOR_INDEX, a); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  // L6470_setparam_kvaldec_index(MAKI_MOTOR_INDEX, a); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  //
	// //	止めとく
  // L6470_softstop();

//////////////////////////////////////////////////////////////////////////////////////////////////////

  //	昇降モーターの設定 12Vversion
  L6470_setparam_acc_index(MAKI_MOTOR_INDEX, 50); //[R, WS] 加速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_dec_index(MAKI_MOTOR_INDEX, 50); //[R, WS] 減速度default 0x08A (12bit) (14.55*val+14.55[step/s^2])
  L6470_setparam_maxspeed_index(MAKI_MOTOR_INDEX, 55); //[R, WR]最大速度default 0x041 (10bit) (15.25*val+15.25[step/s])
  L6470_setparam_minspeed_index(MAKI_MOTOR_INDEX, 0x000); //[R, WS]最小速度default 0x000 (1+12bit) (0.238*val[step/s])
  L6470_setparam_fsspd_index(MAKI_MOTOR_INDEX, 0x3ff); //[R, WR]μステップからフルステップへの切替点速度default 0x027 (10bit) (15.25*val+7.63[step/s])

  L6470_setparam_intspd_index(MAKI_MOTOR_INDEX, 0x489);//0x1ED6
  L6470_setparam_stslp_index(MAKI_MOTOR_INDEX, 0x13A);//0x20
  L6470_setparam_fnslpacc_index(MAKI_MOTOR_INDEX, 0x2C5);//0x50
  L6470_setparam_fnslpdec_index(MAKI_MOTOR_INDEX, 0x2C5);
	L6470_setparam_stepmood_index(MAKI_MOTOR_INDEX, 0x07);	// マイクロステップ 1/128
  L6470_setparam_ocdth_index(MAKI_MOTOR_INDEX, 0x8);
  L6470_setparam_stallth_index(MAKI_MOTOR_INDEX, 0x8);

	//	共通設定
  L6470_setparam_alarmen(0x30);	//	アラームはストールだけ
	L6470_setparam_config(CONFIG_VS_COMP_ENABLE | CONFIG_INT_16MHZ);	//	内蔵クロックで動作、VS-COMP使う

  //  電圧設定
  int a = 0x6B; //  aのモーターのトルク（arduinoから遠い方）ここも96から80とかに下げてみる
  int b = 40; //  回転台のトルク

  L6470_setparam_kvalhold_index(MAKI_MOTOR_INDEX, a); //[R, WR]停止時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalrun_index(MAKI_MOTOR_INDEX, a); //[R, WR]定速回転時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvalacc_index(MAKI_MOTOR_INDEX, a); //[R, WR]加速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)
  L6470_setparam_kvaldec_index(MAKI_MOTOR_INDEX, a); //[R, WR]減速時励磁電圧default 0x29 (8bit) (Vs[V]*val/256)

	//	止めとく
  L6470_softstop();
}
