package {{project_package_name}}.springboot;

import {{project_package_name}}.constants.{{project_module_constant}};
import com.wanshun.net.springboot.WanshunNetBeanConfig;
import com.weibo.api.motan.config.springsupport.BasicRefererConfigBean;
import com.weibo.api.motan.config.springsupport.RefererConfigBean;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class {{project_rpc_client_config}} {
	
    @Bean
    public BasicRefererConfigBean driverStatsRefererConfigBean(){
    	return WanshunNetBeanConfig.getBasicRefererConfigBean({{project_module_constant}}.GROUP_NAME_CLIENT, {{project_module_constant}}.APPLICATION_NAME, {{project_module_constant}}.APPLICATION_NAME);
    }

//    @Bean
//    public RefererConfigBean<RpcTaxiDriverDisStaticService> rpcTaxiDriverDisStaticService(@Qualifier("driverStatsRefererConfigBean") BasicRefererConfigBean driverStatsRefererConfigBean){
//        RefererConfigBean<RpcTaxiDriverDisStaticService> bean = new RefererConfigBean<>();
//        bean.setVersion(RpcTaxiDriverDisStaticService.VERSION);
//        bean.setCheck(false);
//        bean.setInterface(RpcTaxiDriverDisStaticService.class);
//        bean.setBasicReferer(driverStatsRefererConfigBean);
//
//        return bean;
//    }

}
