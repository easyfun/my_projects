package com.wanshun.fileexport.springboot.config.job;

import com.dangdang.ddframe.job.api.simple.SimpleJob;
import com.dangdang.ddframe.job.config.JobCoreConfiguration;
import com.dangdang.ddframe.job.config.JobTypeConfiguration;
import com.dangdang.ddframe.job.config.simple.SimpleJobConfiguration;
import com.dangdang.ddframe.job.event.JobEventConfiguration;
import com.dangdang.ddframe.job.event.rdb.JobEventRdbConfiguration;
import com.dangdang.ddframe.job.lite.api.JobScheduler;
import com.dangdang.ddframe.job.lite.config.LiteJobConfiguration;
import com.dangdang.ddframe.job.lite.spring.api.SpringJobScheduler;
import com.dangdang.ddframe.job.reg.zookeeper.ZookeeperConfiguration;
import com.dangdang.ddframe.job.reg.zookeeper.ZookeeperRegistryCenter;
import com.wanshun.common.database.mysql.DateSourceConfigBean;
import com.wanshun.common.eventproc.ApplicationShutdownHook;
import com.wanshun.common.utils.Base64;
import com.wanshun.common.database.mysql.DataSourceConfigUtils;
import com.wanshun.fileexport.constants.FileExportModuleConstant;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.env.Environment;

import javax.annotation.PostConstruct;
import javax.sql.DataSource;

@Configuration
public class JobConfig {

    public static Logger logger = LoggerFactory.getLogger(JobConfig.class);

    @Autowired
    private Environment environment;

    @Autowired
    private DateSourceConfigBean dateSourceConfigBean;

    @Value("${zk.address}")
    private String serverList;

    @Value("${zk.job.namespace}")
    private String namespace;

//    @Resource
//    private ZookeeperRegistryCenter jobRegCenter;
//
//    @Resource
//    private JobEventConfiguration jobEventConfiguration;

    @Bean(initMethod = "init")
    public ZookeeperRegistryCenter jobRegCenter() {
        String namespace = this.namespace + FileExportModuleConstant.GROUP_NAME_SERVER;
        return new ZookeeperRegistryCenter(new ZookeeperConfiguration(serverList, namespace));
    }

    @Bean(name = "jobDataSource")
    public DataSource jobDataSource() {
        logger.info("JobDatabase config={}", dateSourceConfigBean.toString());
        String url = environment.getProperty("job.druid.url");

        String value = environment.getProperty("job.druid.username");
        String username = Base64.decode(value, "utf-8");

        value = environment.getProperty("job.druid.password");
        String password = Base64.decode(value, "utf-8");

        return DataSourceConfigUtils.buildDruidDataSource(url, username, password, dateSourceConfigBean);
    }

    @Bean
    public JobEventConfiguration jobEventConfiguration(@Autowired DataSource jobDataSource) {
        return new JobEventRdbConfiguration(jobDataSource);
    }

    @PostConstruct
    public void init() {
        ApplicationShutdownHook h = new ApplicationShutdownHook();
        Thread t = new Thread(h);
        Runtime.getRuntime().addShutdownHook(t);
    }

//    @Bean(initMethod = "init")
//    public JobScheduler sumDriverDisStaticJobScheduler(final SimpleJob sumDriverDisStaticJob,
//                                                       @Value("${job.sumDriverDisStaticJob.cron}") final String cron,
//                                                       @Value("${job.sumDriverDisStaticJob.shardingTotalCount}") final int shardingTotalCount,
//                                                       @Value("${job.sumDriverDisStaticJob.shardingItemParameters}") final String shardingItemParameters) {
//        LiteJobConfiguration liteJobConfiguration = getLiteJobConfiguration(sumDriverDisStaticJob.getClass(),
//                cron,
//                shardingTotalCount,
//                shardingItemParameters);
//
//        return new SpringJobScheduler(sumDriverDisStaticJob,
//                jobRegCenter,
//                liteJobConfiguration,
//                jobEventConfiguration);
//    }

    private LiteJobConfiguration getLiteJobConfiguration(final Class<? extends SimpleJob> jobClass,
                                                         final String cron,
                                                         final int shardingTotalCount,
                                                         final String shardingItemParameters) {
        JobCoreConfiguration jobCoreConfiguration = JobCoreConfiguration.newBuilder(jobClass.getName(), cron, shardingTotalCount)
                .shardingItemParameters(shardingItemParameters)
                .build();
        JobTypeConfiguration jobTypeConfiguration = new SimpleJobConfiguration(jobCoreConfiguration, jobClass.getCanonicalName());
        return LiteJobConfiguration.newBuilder(jobTypeConfiguration).overwrite(true).build();
    }
}
