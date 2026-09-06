"""Alternative firmwares for TLB2Z on the TLSR825x/TLSR8656 chipset.
see https://github.com/pvvx/TLB2Z
"""
from zigpy.quirks.v2 import QuirkBuilder, ReportingConfig, EntityType
from zhaquirks import CustomCluster
from zigpy.quirks.v2.homeassistant.sensor import SensorDeviceClass, SensorStateClass
from zigpy.zcl.clusters.measurement import IlluminanceLevelSensing, LevelStatus
from zigpy.zcl.foundation import ZCLAttributeDef, ZCL_REPORTING_STATUS_ATTR
from zigpy.zcl.clusters.general import PowerConfiguration, OnOff
import zigpy.types as t
from zha.units import PERCENTAGE, LIGHT_LUX


def lxLevelStatus_converter(value: int) -> str:
	actions = {
		0: "Average", # "On"
		1: "Low",   # "Below"
		2: "High", # "Above"
		255: "Unknown",
	}
	return actions.get(value)

class TriggerCode(t.enum8):
	Nothing = 0     
	Power_0x10 = 16
	Opened_0x11 = 17
	Door_0x1A = 26
	Garage_Door_0x1B = 27
	Gas_0x1C = 28
	Hot_0x1D = 29
	Light_0x1E = 30
	Locked_0x1F = 31
	Moisture_0x20 = 32
	Motion_0x21 = 33
	Moving_0x22 = 34
	Occupancy_0x23 = 35
	Plug_0x24 = 36
	Presence_0x25 = 37
	Problem_0x26 =  38
	Running_0x27 =  39
	Safe_0x28 = 40
	Smoke_0x29 = 41
	Sound_0x2A = 42
	Tamper_0x2B = 43
	Vibration_0x2C = 44
	Window_0x2D = 45

class CustomOnOff(CustomCluster, OnOff):

	class AttributeDefs(OnOff.AttributeDefs):
		trigger_code = ZCLAttributeDef(
			id=0xf001,
			type=TriggerCode,
			access="rw",
			is_manufacturer_specific=True,
		)
  
(
    QuirkBuilder("B2Z", "REL-BZ01")
    .applies_to("B2Z", "REL-BZ02")
    .replaces(CustomOnOff,  endpoint_id=1)
    .replaces(CustomOnOff,  endpoint_id=2)
    .replaces(CustomOnOff,  endpoint_id=3)
	.enum(
		CustomOnOff.AttributeDefs.trigger_code.name,
		TriggerCode,
		CustomOnOff.cluster_id,
		entity_type=EntityType.CONFIG,
		translation_key="trigger_code",
		fallback_name="Trigger1",
		endpoint_id=1,
	)
	.enum(
		CustomOnOff.AttributeDefs.trigger_code.name,
		TriggerCode,
		CustomOnOff.cluster_id,
		entity_type=EntityType.CONFIG,
		translation_key="trigger_code",
		fallback_name="Trigger2",
		endpoint_id=2,
	)
	.enum(
		CustomOnOff.AttributeDefs.trigger_code.name,
		TriggerCode,
		CustomOnOff.cluster_id,
		entity_type=EntityType.CONFIG,
		translation_key="trigger_code",
		fallback_name="Trigger3",
		endpoint_id=3,
	)
    .number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		fallback_name="zlx target1",
		mode="box",
		endpoint_id=1,
	)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		translation_key="level_status",
		fallback_name="Light level1",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
		endpoint_id=1,
	)
    .number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		fallback_name="zlx target2",
		mode="box",
		endpoint_id=2,
	)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		translation_key="level_status",
		fallback_name="Light level2",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
		endpoint_id=2,
	)
    .number(
		IlluminanceLevelSensing.AttributeDefs.illuminance_target_level.name,
		IlluminanceLevelSensing.cluster_id,
		min_value=0,
		max_value=65535,
		step=1,
		translation_key="illuminance_target_level",
		fallback_name="zlx target3",
		mode="box",
		endpoint_id=3,
	)
	.sensor(
		IlluminanceLevelSensing.AttributeDefs.level_status.name,
		IlluminanceLevelSensing.cluster_id,
		attribute_converter=lxLevelStatus_converter,
		translation_key="level_status",
		fallback_name="Light level3",
		reporting_config=ReportingConfig(
			min_interval=0,
			max_interval=3600,
			reportable_change=1,
		),
		endpoint_id=3,
	)
    .sensor(
        attribute_name=PowerConfiguration.AttributeDefs.battery_2_percentage_remaining.name,
        cluster_id=PowerConfiguration.cluster_id,
        device_class=SensorDeviceClass.BATTERY,
        state_class=SensorStateClass.MEASUREMENT,
        unit=PERCENTAGE,
        divisor=2, 
        fallback_name="Battery2",
        translation_key="battery2",
        entity_type=EntityType.DIAGNOSTIC,
		reporting_config=ReportingConfig(
			min_interval=600,
			max_interval=3600,
			reportable_change=1,
		),
		endpoint_id=1,
    )
    .sensor(
        attribute_name=PowerConfiguration.AttributeDefs.battery_3_percentage_remaining.name,
        cluster_id=PowerConfiguration.cluster_id,
        device_class=SensorDeviceClass.BATTERY,
        state_class=SensorStateClass.MEASUREMENT,
        unit=PERCENTAGE,
        divisor=2, 
        fallback_name="Battery3",
        translation_key="battery3",
        entity_type=EntityType.DIAGNOSTIC,
		reporting_config=ReportingConfig(
			min_interval=600,
			max_interval=3600,
			reportable_change=1,
		),
		endpoint_id=1,
    )
    .add_to_registry()
)
