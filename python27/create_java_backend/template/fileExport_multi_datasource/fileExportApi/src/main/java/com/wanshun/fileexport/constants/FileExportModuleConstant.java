package com.wanshun.fileexport.constants;


import com.wanshun.net.motan.MotanConstConfig;

public class FileExportModuleConstant {

	public static final String APPLICATION_NAME = MotanConstConfig.FILE_EXPORT_SERVER_NAME;

	public static final String GROUP_NAME_SERVER = APPLICATION_NAME + MotanConstConfig.getGroupName();

	public static final String GROUP_NAME_CLIENT = APPLICATION_NAME + MotanConstConfig.getGroupName(APPLICATION_NAME);

}
