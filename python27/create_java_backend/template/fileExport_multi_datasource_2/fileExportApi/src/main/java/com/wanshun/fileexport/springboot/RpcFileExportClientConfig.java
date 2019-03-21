package com.wanshun.fileexport.springboot;

import com.wanshun.fileexport.constants.FileExportModuleConstant;
import com.wanshun.net.springboot.WanshunNetBeanConfig;
import com.weibo.api.motan.config.springsupport.BasicRefererConfigBean;
import com.weibo.api.motan.config.springsupport.RefererConfigBean;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RpcFileExportClientConfig {
	
    @Bean
    public BasicRefererConfigBean driverStatsRefererConfigBean(){
    	return WanshunNetBeanConfig.getBasicRefererConfigBean(FileExportModuleConstant.GROUP_NAME_CLIENT, FileExportModuleConstant.APPLICATION_NAME, FileExportModuleConstant.APPLICATION_NAME);
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
