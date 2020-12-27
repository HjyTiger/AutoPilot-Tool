#ifndef LCM_CHANNEL_H_
#define LCM_CHANNEL_H_

const std::string CHANNEL_NAME_CAN_MSG="CAN";
const std::string CHANNEL_NAME_CONTROL_CMD="CONTROL_CMD";
const std::string CHANNEL_NAME_PIKE_IMAGE="PIKE_IMAGE";
const std::string CHANNEL_NAME_VCU_550="VCU_550";

const std::string CHANNEL_NAME_EXECUTE_TRAJECTORY="EXECUTE_TRAJECTORY";
const std::string CHANNEL_NAME_FITTED_TRAJECTORY="FITTED_TRAJECTORY";
const std::string CHANNEL_NAME_VCU_VEHICLE_INFO="VCU_VEHICLE_INFO";
const std::string CHANNEL_NAME_EXECUTE_BEHAVIOR_PLAN="EXECUTE_BEHAVIOR_PLAN";
//ins related channels;
//=======================================================================================
const std::string CHANNEL_NAME_INERTIAL="INERTIAL";
const std::string CHANNEL_NAME_SPATIAL_FOG="SPATIAL_FOG";
const std::string CHANNEL_NAME_RAW_INS="RAW_INS";
const std::string CHANNEL_NAME_TRANSFORM="TRANSFORM";
//=======================================================================================


//mwr related channels;
//=======================================================================================
const std::string CHANNEL_NAME_MWR_RAW_FRONT_OBJECTS="MWR_RAW_FRONT_OBJECTS";
const std::string CHANNEL_NAME_MWR_RAW_REAR_ADAS_MSG="MWR_RAW_REAR_ADAS_MSG";
const std::string CHANNEL_NAME_MWR_RAW_REAR_OBJECTS_FROM_PCAN_IN_SENSOR_COORDINATE="MWR_RAW_REAR_OBJECTS_FROM_PCAN_IN_SENSOR_COORDINATE";
const std::string CHANNEL_NAME_MWR_RAW_LEFT_OBJECTS_FROM_TCPIP_IN_RECTANGULAR_COORDINATE="MWR_RAW_LEFT_OBJECTS_FROM_TCPIP_IN_RECTANGULAR_COORDINATE";
const std::string CHANNEL_NAME_MWR_RAW_RIGHT_OBJECTS_FROM_TCPIP_IN_RECTANGULAR_COORDINATE="MWR_RAW_RIGHT_OBJECTS_FROM_TCPIP_IN_RECTANGULAR_COORDINATE";
const std::string CHANNEL_NAME_MWR_FILTERED_FRONT_OBJECTS="MWR_FILTERED_FRONT_OBJECTS";
const std::string CHANNEL_NAME_MWR_FILTERED_LEFT_OBJECTS="MWR_FILTERED_LEFT_OBJECTS";
const std::string CHANNEL_NAME_MWR_FILTERED_RIGHT_OBJECTS="MWR_FILTERED_RIGHT_OBJECTS";

const std::string CHANNEL_NAME_MWR_SELECTED_FRONT_OBJECTS="MWR_SELECTED_FRONT_OBJECTS";
const std::string CHANNEL_NAME_MWR_SELECTED_LEFT_OBJECTS="MWR_SELECTED_LEFT_OBJECTS";
const std::string CHANNEL_NAME_MWR_SELECTED_RIGHT_OBJECTS="MWR_SELECTED_RIGHT_OBJECTS";
//=======================================================================================


//mobileye related channels;
//=======================================================================================
const std::string CHANNEL_NAME_MOBILEYE_LANES="MOBILEYE_LANES";
const std::string CHANNEL_NAME_MOBILEYE_HPP="MOBILEYE_HPP";
const std::string CHANNEL_NAME_MOBILEYE_OBJECTS="MOBILEYE_OBJECTS";
const std::string CHANNEL_NAME_MOBILEYE_FREESPACE="MOBILEYE_FREESPACE";
const std::string CHANNEL_NAME_MOBILEYE_VIRTUAL_LANES="MOBILEYE_VIRTUAL_LANES";
const std::string CHANNEL_NAME_MOBILEYE_TFLS="MOBILEYE_TFLS";
const std::string CHANNEL_NAME_MOBILEYE_FILTERED_OBJECTS="MOBILEYE_FILTERED_OBJECTS";
//=======================================================================================

//ibeo related channels;
//=======================================================================================
const std::string CHANNEL_NAME_LUX_POINTS_FRONT="LUX_POINTS_FRONT";
const std::string CHANNEL_NAME_LUX_POINTS_LEFT="LUX_POINTS_LEFT";
const std::string CHANNEL_NAME_LUX_POINTS_BACK="LUX_POINTS_BACK";
const std::string CHANNEL_NAME_LUX_POINTS_RIGHT="LUX_POINTS_RIGHT";
const std::string CHANNEL_NAME_LUX_OBJECTS_FRONT="LUX_OBJECTS_FRONT";
const std::string CHANNEL_NAME_LUX_OBJECTS_LEFT="LUX_OBJECTS_LEFT";
const std::string CHANNEL_NAME_LUX_OBJECTS_BACK="LUX_OBJECTS_BACK";
const std::string CHANNEL_NAME_LUX_OBJECTS_RIGHT="LUX_OBJECTS_RIGHT";
const std::string CHANNEL_NAME_IBEO_OBJECTS="IBEO_OBJECTS";
const std::string CHANNEL_NAME_IBEO_POINTS="IBEO_POINTS";
const std::string CHANNEL_NAME_IBEO_FILTERED_OBJECTS="IBEO_FILTERED_OBJECTS";
const std::string CHANNEL_NAME_IBEO_ROAD_EDGE_PROB ="IBEO_ROADEDGE_PROB";
//=======================================================================================

//velodyne related channels;
//=======================================================================================
const std::string CHANNEL_NAME_VELODYNE64="VELODYNE64";
const std::string CHANNEL_NAME_VELODYNE64_COMPENSATE="VELODYNE64_COMPENSATE";
const std::string CHANNEL_NAME_VELODYNE_GROUND_ESTIMATION="VELODYNE_GROUND_ESTIMATION";
const std::string CHANNEL_NAME_VELODYNE_OBJECT_CLUSTERS="VELODYNE_OBJECT_CLUSTERS";
const std::string CHANNEL_NAME_VELODYNE_OBJECT_CONVEX_HULLS="VELODYNE_OBJECTS_CONVEX_HULLS";
const std::string CHANNEL_NAME_VELODYNE_TRANSFORMED_CONVEX_HULLS="VELODYNE_OBJECTS_TRANSFORMED_CONVEX_HULLS";
const std::string CHANNEL_NAME_VELODYNE_OBJECTS_CORRELATIONS="VELODYNE_OBJECT_CORRELATIONS";
const std::string CHANNEL_NAME_VELODYNE_OBJECTS="VELODYNE_OBJECTS";
const std::string CHANNEL_NAME_VELODYNE_GRIDMAP_LABEL="VELODYNE_GRIDMAP_LABEL";
const std::string CHANNEL_NAME_VELODYNE_720POINTS="VELODYNE_720POINTS";
//=======================================================================================

//can2 proxy related channels;
//=======================================================================================
const std::string CHANNEL_NAME_CAN2_FRONTMWR2IPC="CAN2_FRONTMWR2IPC";
const std::string CHANNEL_NAME_CAN2_VCU2IPC="CAN2_VCU2IPC";
const std::string CHANNEL_NAME_CAN2_IPC2VCU_GPS="CAN2_IPC2VCU_GPS";
const std::string CHANNEL_NAME_CAN2_IPC2VCU_TRAJECTORY="CAN2_IPC2VCU_TRAJECTORY";
const std::string CHANNEL_NAME_CAN2_IPC2VCU_MELANES="CAN2_IPC2VCU_MELANES";
//=======================================================================================


//iboe perception oct obstacles;
//=======================================================================================
const std::string CHANNEL_NAME_PERCEPTION_OCT_OBSTACLES="PERCEPTION_OCT_OBSTACLES";
//const std::string CHANNEL_NAME_PERCEPTION_OBJECTS="PERCEPTION_OBJECTS";
const std::string CHANNEL_NAME_PERCEPTION_OBSTACLES="PERCEPTION_OBSTACLES";
const std::string CHANNEL_NAME_PERCEPTION_HPP="PERCEPTION_HPP";
const std::string CHANNEL_NAME_PERCEPTION_TSR="PERCEPTION_TSR";
const std::string CHANNEL_NAME_PERCEPTION_LINES="PERCEPTION_LINES";
const std::string CHANNEL_NAME_IBEO_SELECTED_OBJECTS="IBEO_SELECTED_OBJECTS";
const std::string CHANNEL_NAME_TRACKED_OBJECTS="TRACKED_OBJECTS";
//=======================================================================================
//HD Map related
//=======================================================================================
const std::string CHANNEL_NAME_DRIVER_HDMAP="DRIVER_HDMAP";
const std::string CHANNEL_NAME_DRIVER_MAP="DRIVER_MAP";
const std::string CHANNEL_NAME_DRIVER_MAP_LANE="DRIVER_MAP_LANE";
const std::string CHANNEL_NAME_DRIVER_MAP_TSR_DISTANCE = "DRIVER_MAP_TSR_DISTANCE";
const std::string CHANNEL_NAME_DRIVER_MAP_CHAIN_FURNITURE = "DRIVER_CHAIN_FURNITURE";


//freespace

const std::string CHANNEL_NAME_IBEO_FREE_SPACE = "IBEO_FREE_SPACE";
const std::string CHANNEL_NAME_IBEO_FILTERED_FREE_SPACE = "IBEO_FILTERED_FREE_SPACE";
const std::string CHANNEL_NAME_IBEO_FREE_SPACE_SCALE = "IBEO_FREE_SPACE_SCALE";
const std::string CHANNEL_NAME_IBEO_FILTERED_FREE_SPACE_SCALE = "IBEO_FILTERED_FREE_SPACE_SCALE";
const std::string CHANNEL_NAME_IBEO_NEW_FREE_SPACE = "IBEO_NEW_FREE_SPACE";
const std::string CHANNEL_NAME_PERCEPTION_FREESPACE = "PERCEPTION_FREESPACE";
//=======================================================================================
enum COORINDATE_FRMAE_ID
{
    CFI_UNKONWN,
    CFI_WORLD,
    CFI_VEHICLE,
    CFI_VELODYNE64,
    CFI_MOBILEYE,    
    CFI_FRONT_MWR,
    CFI_LEFT_MWR,
    CFI_RIGHT_MWR,
    CFI_PIKE,
    CFI_LUX_FRONT,
    CFI_LUX_LEFT,
    CFI_LUX_BACK,
    CFI_LUX_RIGHT,
    CFI_IBEO,
    CFI_WORLD_VCUDR
};

/*header example
//initialize example:
m_VelodyneFrame.header.nTimeStamp = GetGlobalTimeStampInMicroSec();
m_VelodyneFrame.header.nCounter = 0;
m_VelodyneFrame.header.nFrameID = CFI_VELODYNE64;//velodyne frame;
//publish example:
m_VelodyneFrame.header.nCounter = (static_cast<unsigned int>(m_VelodyneFrame.header.nCounter)+1)%65535;
m_VelodyneFrame.header.nTimeStamp = GetGlobalTimeStampInMicroSec();
//publish message example;
VELODYNE64_FRAME m_VelodyneFrame;
lcm::LCM* m_pLCM;
m_pLCM->publish(CHANNEL_NAME_VELODYNE64, &m_VelodyneFrame);
*/
#endif
