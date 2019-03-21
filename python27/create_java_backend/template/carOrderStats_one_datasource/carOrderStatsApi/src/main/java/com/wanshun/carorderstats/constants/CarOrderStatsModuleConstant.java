package {{project_package_name}}.constants;


import com.wanshun.net.motan.MotanConstConfig;

public class {{project_module_constant}} {

	public static final String APPLICATION_NAME = MotanConstConfig.{{project_application_name}};

	public static final String GROUP_NAME_SERVER = APPLICATION_NAME + MotanConstConfig.getGroupName();

	public static final String GROUP_NAME_CLIENT = APPLICATION_NAME + MotanConstConfig.getGroupName(APPLICATION_NAME);

}
