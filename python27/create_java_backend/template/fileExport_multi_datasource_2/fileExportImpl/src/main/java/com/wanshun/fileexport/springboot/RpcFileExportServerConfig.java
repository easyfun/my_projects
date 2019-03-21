package com.wanshun.fileexport.springboot;

import com.wanshun.fileexport.constants.FileExportModuleConstant;
import com.wanshun.net.springboot.WanshunNetBeanConfig;
import com.weibo.api.motan.config.springsupport.BasicServiceConfigBean;
import com.weibo.api.motan.config.springsupport.ServiceConfigBean;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class RpcFileExportServerConfig {
    public static final String MOTAN_FILE_EXPORT_RPC_PORT = "10510";

    @Bean
    public BasicServiceConfigBean serviceBasicConfig(){
        BasicServiceConfigBean bean = new BasicServiceConfigBean();

        bean.setGroup(FileExportModuleConstant.GROUP_NAME_SERVER);
        bean.setModule(FileExportModuleConstant.APPLICATION_NAME);
        bean.setApplication(FileExportModuleConstant.APPLICATION_NAME);

        bean.setAccessLog(false);
        bean.setShareChannel(true);
        bean.setRegistry(WanshunNetBeanConfig.MOTAN_RPC_REGISTRY_NAME);
        bean.setExport(WanshunNetBeanConfig.MOTAN_RPC_SERVERPROTOCOL_NAME + ":" + MOTAN_FILE_EXPORT_RPC_PORT);

        return bean;
    }

//    @Bean
//    public ServiceConfigBean<RpcTaxiDriverDisStaticService> rpcTaxiDriverDisStaticService(@Autowired RpcTaxiDriverDisStaticService rpcTaxiDriverDisStaticService){
//        ServiceConfigBean<RpcTaxiDriverDisStaticService> bean = new ServiceConfigBean<>();
//
//        bean.setInterface(RpcTaxiDriverDisStaticService.class);
//        bean.setVersion(RpcTaxiDriverDisStaticService.VERSION);
//        bean.setRef(rpcTaxiDriverDisStaticService);
//
//        return bean;
//    }

}
