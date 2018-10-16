#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <string.h>
#include <HCNetSDK.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    isLogin(false)
{
    ui->setupUi(this);
    ui->RealPlayBtn->setEnabled(false);
}

MainWindow::~MainWindow()
{
    isLogin = false;
    delete ui;
}

bool MainWindow::initHIKSDK()
{
    //must config before initsdk
    DWORD dwErrCode =0;
    NET_DVR_INIT_CHECK_MODULE_COM struInitCheckModuleCom;
    memset(&struInitCheckModuleCom,0,sizeof(NET_DVR_INIT_CHECK_MODULE_COM));
    struInitCheckModuleCom.byEnable = 1;//默认启用
    if(!NET_DVR_SetSDKInitCfg(NET_SDK_INIT_CFG_TYPE_CHECK_MODULE_COM,&struInitCheckModuleCom))
    {
        dwErrCode = NET_DVR_GetLastError();
    }
    //布防默认最大为2048路
    NET_DVR_INIT_CFG_ABILITY struCfgAbility = { INIT_CFG_NUM_2048, INIT_CFG_NUM_2048, 0 };
    if(!NET_DVR_SetSDKInitCfg(NET_SDK_INIT_CFG_ABILITY,&struCfgAbility))
    {
        dwErrCode = NET_DVR_GetLastError();
    }
    // init sdk
    if(!NET_DVR_Init())
    {
        dwErrCode = NET_DVR_GetLastError();
        return false;
    }
    return true;
}

long MainWindow::loginDevice()
{
    // read config parmeter;

    std::string sIPAddress,sUserName,sPassword;
    WORD wPort;
    DWORD dwErrCode;
    long lUserID;

    sIPAddress = ui->DeviceIPAdd->text().toStdString();
    sUserName  = ui->UserName->text().toStdString();
    sPassword  = ui->Password->text().toStdString();
    wPort      = (WORD)ui->PortSpinBox->value();
    //注册
    g_lUserID;
    //config
    NET_DVR_USER_LOGIN_INFO struUserInfo;
    memset(&struUserInfo,0,sizeof(NET_DVR_USER_LOGIN_INFO));
    struUserInfo.bUseAsynLogin = 0; // 同步登陆方式
    strcpy(struUserInfo.sDeviceAddress,sIPAddress.data());
    strcpy(struUserInfo.sUserName,sUserName.data());
    strcpy(struUserInfo.sPassword,sPassword.data());
    struUserInfo.wPort = wPort;

    //Device infomation  output for parmeter
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = {0};
    lUserID = NET_DVR_Login_V40(&struUserInfo,&struDeviceInfoV40);

    return lUserID;
}

void MainWindow::on_LoginBtn_clicked()
{
    //初始化
    if(!initHIKSDK())return;

    //登陆
    g_lUserID = loginDevice();
    if(g_lUserID<0)
    {
        //printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        ui->statusBar->showMessage(tr("login failed"));
        NET_DVR_Cleanup();
        return ;
    }

    if(g_lUserID == NET_DVR_NOERROR)
    {
       ui->RealPlayBtn->setEnabled(true);
    }
    ui->statusBar->showMessage(tr("logining"));
}

void MainWindow::on_RealPlayBtn_clicked()
{
    //预览
    LONG lRealPlayHandle;

    HWND displayWND =(HWND) ui->displayFrame->winId();

    NET_DVR_PREVIEWINFO struPlayInfo = {0};
    struPlayInfo.hPlayWnd = displayWND;         //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    struPlayInfo.lChannel     = 1;       //预览通道号
    struPlayInfo.dwStreamType = 0;       //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
    struPlayInfo.dwLinkMode   = 0;       //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
    struPlayInfo.bBlocked     = 1;       //0- 非阻塞取流，1- 阻塞取流

    lRealPlayHandle = NET_DVR_RealPlay_V40(g_lUserID, &struPlayInfo, NULL, NULL);
    if(lRealPlayHandle < 0)
    {
        ui->statusBar->showMessage(tr("RealPlay error"));
        NET_DVR_Logout(g_lUserID);
        NET_DVR_Cleanup();
        return ;
    }
}
