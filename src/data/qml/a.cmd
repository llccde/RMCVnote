@echo off

REM 创建主目录
mkdir "CloudFileManager"
cd "CloudFileManager"

REM 创建components目录
mkdir "components"
cd "components"

REM 创建组件文件
echo. > HeaderToolbar.qml
echo. > FileListPanel.qml
echo. > SortToolbar.qml
echo. > FileItem.qml
echo. > DownloadButton.qml
echo. > FileDetailPanel.qml
echo. > UploadRecordList.qml
echo. > UploadRecordItem.qml
echo. > PullRecordList.qml
echo. > PullRecordItem.qml
echo. > CollapseButton.qml
echo. > NetworkStatusIndicator.qml
echo. > AccountPanel.qml

REM 返回上级目录
cd ..

REM 创建主文件
echo. > CloudFileManagerPage.qml
echo. > main.qml

REM 创建头文件（C++文件）
echo. > cloudfilemanageradapter.h

echo 文件创建完成！
pause