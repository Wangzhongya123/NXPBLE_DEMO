PRIMARY_SERVICE(service_gatt, gBleSig_GenericAttributeProfile_d)
        CHARACTERISTIC(char_service_changed, gBleSig_GattServiceChanged_d, (gGattCharPropRead_c | gGattCharPropNotify_c) )
            VALUE(value_service_changed, gBleSig_GattServiceChanged_d, (gPermissionNone_c), 4, 0x00, 0x00, 0x00, 0x00)
            CCCD(cccd_service_changed)

PRIMARY_SERVICE(service_gap, gBleSig_GenericAccessProfile_d)
    CHARACTERISTIC(char_device_name, gBleSig_GapDeviceName_d, (gGattCharPropRead_c) )
            VALUE(value_device_name, gBleSig_GapDeviceName_d, (gPermissionFlagReadable_c), 12, "WZY_BLE_TEST")
    CHARACTERISTIC(char_appearance, gBleSig_GapAppearance_d, (gGattCharPropRead_c) )
            VALUE(value_appearance, gBleSig_GapAppearance_d, (gPermissionFlagReadable_c), 2, UuidArray(gGenericHeartrateSensor_c))
    CHARACTERISTIC(char_ppcp, gBleSig_GapPpcp_d, (gGattCharPropRead_c) )
            VALUE(value_ppcp, gBleSig_GapPpcp_d, (gPermissionFlagReadable_c), 8, 0x0A, 0x00, 0x10, 0x00, 0x64, 0x00, 0xE2, 0x04)

PRIMARY_SERVICE_UUID128(service_qpps, uuid_service_qpps)
    CHARACTERISTIC_UUID128(char_qpps_rx, uuid_qpps_characteristics_rx, (gGattCharPropWriteWithoutRsp_c) )
        VALUE_UUID128_VARLEN(value_qpps_rx, uuid_qpps_characteristics_rx, (gPermissionFlagWritable_c), 64, 1, 0x00)
    CHARACTERISTIC_UUID128(char_qpps_tx, uuid_qpps_characteristics_tx, (gGattCharPropNotify_c))
        VALUE_UUID128_VARLEN(value_qpps_tx, uuid_qpps_characteristics_tx, (gPermissionNone_c), 64, 2, 0x00, 0xB4)	
        CCCD(cccd_qpps_tx)		
	CHARACTERISTIC_UUID128(char_locking_rx, uuid_locking_characteristics_rx, (gGattCharPropWrite_c) )//modify by wzy  锁定状态设置
        VALUE_UUID128_VARLEN(value_locking_rx, uuid_locking_characteristics_rx, (gPermissionFlagWritable_c), 32, 1, 0x00)//modify by wzy	锁定的状态设置	
	CHARACTERISTIC_UUID128(char_dps310_tx, uuid_dps310_characteristics_tx, (gGattCharPropNotify_c))//modify by wzy  DPS数据读取
        VALUE_UUID128_VARLEN(value_dps310_tx, uuid_dps310_characteristics_tx, (gPermissionNone_c), 32, 2, 0x00, 0x00)	//modify by wzy	DPS数据读取
        CCCD(cccd_dsp310_tx)	
	CHARACTERISTIC_UUID128(char_energy_tx, uuid_energy_characteristics_tx, (gGattCharPropNotify_c))//modify by wzy  抽烟能量数据 上传
        VALUE_UUID128_VARLEN(value_energy_tx, uuid_energy_characteristics_tx, (gPermissionNone_c), 32, 2, 0x00, 0x00)	//modify by wzy	抽烟能量数据 上传
        CCCD(cccd_energy_tx)	
	CHARACTERISTIC_UUID128(char_power_tx, uuid_power_characteristics_tx, (gGattCharPropNotify_c))//modify by wzy  抽烟能量数据 上传
        VALUE_UUID128_VARLEN(value_power_tx, uuid_power_characteristics_tx, (gPermissionNone_c), 32, 2, 0x00, 0x00)	//modify by wzy	抽烟能量数据 上传
        CCCD(cccd_power_tx)	
	CHARACTERISTIC_UUID128(char_workmode_tx, uuid_workmode_characteristics_tx, (gGattCharPropNotify_c | gGattCharPropRead_c))//modify by wzy 当前工作状态 上传
        VALUE_UUID128_VARLEN(value_workmode_tx, uuid_workmode_characteristics_tx, (gPermissionFlagReadable_c), 32, 2, 0x00, 0x00)	//modify by wzy	当前工作状态  上传
        CCCD(cccd_workmode_tx)			

PRIMARY_SERVICE_UUID128(service_ecig, uuid_service_ecig)
    CHARACTERISTIC_UUID128(char_ecig_command_rx, uuid_ecig_command_characteristics_rx, (gGattCharPropWrite_c) )//modify by wzy
        VALUE_UUID128_VARLEN(value_ecig_command_rx, uuid_ecig_command_characteristics_rx, (gPermissionFlagWritable_c), 32, 1, 0x00)//modify by wzy
    CHARACTERISTIC_UUID128(char_ecig_data_rx, uuid_ecig_data_characteristics_rx, (gGattCharPropWriteWithoutRsp_c) )//modify by wzy
        VALUE_UUID128_VARLEN(value_ecig_data_rx, uuid_ecig_data_characteristics_rx, (gPermissionFlagWritable_c), 32, 1, 0x00)//modify by wzy		
    CHARACTERISTIC_UUID128(char_ecig_data_tx, uuid_ecig_data_characteristics_tx, (gGattCharPropNotify_c))//modify by wzy
        VALUE_UUID128_VARLEN(value_ecig_data_tx, uuid_ecig_data_characteristics_tx, (gPermissionNone_c), 32, 2, 0x00, 0xB4)//modify by wzy
		CCCD(cccd_ecig_data_tx)
    CHARACTERISTIC_UUID128(char_ecig_command_tx, uuid_ecig_command_characteristics_tx, (gGattCharPropNotify_c))//modify by wzy
        VALUE_UUID128_VARLEN(value_ecig_command_tx, uuid_ecig_command_characteristics_tx, (gPermissionNone_c), 32, 2, 0x00, 0x00)	//modify by wzy	
        CCCD(cccd_ecig_command_tx)	


PRIMARY_SERVICE(service_battery, gBleSig_BatteryService_d)
    CHARACTERISTIC(char_battery_level, gBleSig_BatteryLevel_d, (gGattCharPropNotify_c | gGattCharPropRead_c))
        VALUE(value_battery_level, gBleSig_BatteryLevel_d, (gPermissionFlagReadable_c), 1, 0x5A)
        DESCRIPTOR(desc_bat_level, gBleSig_CharPresFormatDescriptor_d, (gPermissionFlagReadable_c), 7, 0x04, 0x00, 0xAD, 0x27, 0x01, 0x01, 0x00)
        CCCD(cccd_battery_level)

PRIMARY_SERVICE(service_device_info, gBleSig_DeviceInformationService_d)
    CHARACTERISTIC(char_manuf_name, gBleSig_ManufacturerNameString_d, (gGattCharPropRead_c) )
        VALUE(value_manuf_name, gBleSig_ManufacturerNameString_d, (gPermissionFlagReadable_c), sizeof(MANUFACTURER_NAME), MANUFACTURER_NAME)
    CHARACTERISTIC(char_model_no, gBleSig_ModelNumberString_d, (gGattCharPropRead_c) )
        VALUE(value_model_no, gBleSig_ModelNumberString_d, (gPermissionFlagReadable_c), 9, "QPPS Demo")
    CHARACTERISTIC(char_serial_no, gBleSig_SerialNumberString_d, (gGattCharPropRead_c) )
        VALUE(value_serial_no, gBleSig_SerialNumberString_d, (gPermissionFlagReadable_c), 7, "BLESN01")
    CHARACTERISTIC(char_hw_rev, gBleSig_HardwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_hw_rev, gBleSig_HardwareRevisionString_d, (gPermissionFlagReadable_c), sizeof(BOARD_NAME), BOARD_NAME)
    CHARACTERISTIC(char_fw_rev, gBleSig_FirmwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_fw_rev, gBleSig_FirmwareRevisionString_d, (gPermissionFlagReadable_c), 5, "1.1.1")
    CHARACTERISTIC(char_sw_rev, gBleSig_SoftwareRevisionString_d, (gGattCharPropRead_c) )
        VALUE(value_sw_rev, gBleSig_SoftwareRevisionString_d, (gPermissionFlagReadable_c), 5, "1.1.4")
    CHARACTERISTIC(char_system_id, gBleSig_SystemId_d, (gGattCharPropRead_c) )
        VALUE(value_system_id, gBleSig_SystemId_d, (gPermissionFlagReadable_c), 8, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0x9F, 0x04, 0x00)
    CHARACTERISTIC(char_rcdl, gBleSig_IeeeRcdl_d, (gGattCharPropRead_c) )
        VALUE(value_rcdl, gBleSig_IeeeRcdl_d, (gPermissionFlagReadable_c), 4, 0x00, 0x00, 0x00, 0x00)

