#ifndef CF_INTERFACE_H
#define CF_INTERFACE_H

namespace DEBUGTOOL
{

enum ETagGroup
{
    ETG_SENCE = 0,
    ETG_SENSOR_OBJECT,
    ETG_SENSOR_ISSUE,
    ETG_VECHICLE_ISSUE,
    ETG_INTERVENTION,
};

enum EDirection
{
    ED_LEFT = 0,
    ED_RIGHT,
};

enum EAddress
{
    EA_POINT_A = 0,
    EA_POINT_B,
    EA_GATE_BRAKE_POINT_A,
    EA_GATE_BRAKE_POINT_B,
    EA_BRIDGE,
};

enum ETakeover
{
    ETO_YES = 0,
    ETO_NO,
};

// top level issue edit tye
enum EIssueEditType
{
    EIT_DEFAULT_NO = 0,
    EIT_EDIT,
    EIT_COMBO_EDIT,
    EIT_ISSUEADD_SUPPLY_CUSTOM,
    EIT_SUPPLY_CUSTOM,

    EIT_PARENT_END = 10,

    EIT_EDIT_IN_ISSUE = 11,
    EIT_COMBO_IN_ISSUE,
};

enum EBaseInfoType
{
    EBT_DEFAULT_NO = 0,

    EBT_ADDRESS,
    EBT_DIRECTION,
    EBT_TAKEOVER,
    EBT_SWC,

//    EBT_TAG_HIDE = 70,

    EBT_CUSTOM = 90,
};

// show test tag edit table
enum ETestTagEditColumns
{
    TEC_Start_Time = 0,
    TEC_End_Time,
    TEC_Issue,
    TEC_Log_File_Name,
    TEC_Log_File_Path,


    TEC_End,
};

// test base info, for saved
enum ETestBaseInfos
{
    TBI_Project = 0,
    TBI_SW_Version_No,
    TBI_Vehicle_No,
    TBI_Tester,
    TBI_Disk_No,

};


enum EConfigDialogType
{
    CDT_Option = 0,
    CDT_TestTag_Edit,
    CDT_TestTag_CreateNew,
    CDT_Compress_Setting,
};




const static int MAX_TEST_TAG_FILE_COUNT = 2000;
const static int TEST_TAG_FILE_REMOVE_COUNT = 200;

const static int MIN_TEST_TAG_TREE_COLUMN = 2;
const static int MAX_TEST_TAG_TREE_COLUMN = 6;

const static int MAX_PLAYER_VALUE = 1e6;

const static char * LOG_SUFFIX = ".log";
const static char * COMPRESS_SUFFIX = ".tar.xz";


const static char* LOG_SUBDIR = "log_data";
const static char* LOG_ISSUE_SUBDIR = "issue_log";


const static char* TEST_TAG_FILE_NAME = "log-test-tag-record.txt";

//// delay time set, seconds
//static short ISSUE_CHECK_MAX_TIME_DELAY = 3;

//// enable click tag label
//static bool  CLICK_TAG_LABLE_ENABLED = false;

//// enable create new project
//static bool CREATE_NEW_PROJECT_ENABLED = false;

//// enable create/add new test tag
//static bool CREATE_NEW_TESTTAG_ENABLED = false;

//// set tree style
//static bool TEST_TAG_TREE_COLUMN_STYLE;


enum EConfigFileNodeType
{
    CNT_DEFAULT = 0,

    CNT_Project_Base ,
    CNT_Tag_Base_Infos,
    CNT_Tag_Items,

};

enum EProjectFileNodeType
{
    PNT_DEFAULT = 0,

    PNT_Project_Default,
    PNT_Logger_Default,

    PNT_TestBaseInfo_Default,
    PNT_CompressSetting_Default,

};


//class IItemInfo
//{
//   public:
//    virtual ~IItemInfo() = 0 {};

//};

}


#endif
