package com.wanshun.fileexport.springboot.config;

import com.wanshun.common.filesystem.FileSystemClient;
import com.wanshun.common.filesystem.FileSystemClientFactory;
import com.wanshun.common.filesystem.FileSystemConfigBean;
import org.csource.common.MyException;
import org.csource.fastdfs.ClientGlobal;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

import java.io.IOException;
import java.util.Properties;

@Configuration
public class FileSystemClientConfig {
    @Autowired
    private FileSystemConfigBean fileSystemConfigBean;

    @Bean
    public FileSystemClient fileSystemClient() throws IOException, MyException {
        /**
         * 不利于扩展
         */
        Properties properties = new Properties();
        String key = ClientGlobal.PROP_KEY_CONNECT_TIMEOUT_IN_SECONDS;
        properties.put(key, fileSystemConfigBean.getConnectTimeoutInSeconds());

        key = ClientGlobal.PROP_KEY_NETWORK_TIMEOUT_IN_SECONDS;
        properties.put(key, fileSystemConfigBean.getNetworkTimeoutInSeconds());

        key = ClientGlobal.PROP_KEY_CHARSET;
        properties.put(key, fileSystemConfigBean.getCharset());

        key = ClientGlobal.PROP_KEY_TRACKER_SERVERS;
        properties.put(key, fileSystemConfigBean.getTrackerServers());

        return FileSystemClientFactory.build(FileSystemClientFactory.FAST_DFS, properties);
    }
}
