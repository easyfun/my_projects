package com.wanshun.poolfile.springboot;

import com.wanshun.poolfile.service.ExecuteService;
import com.wanshun.poolfile.service.switchds.driver.DriverService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.aop.Advisor;
import org.springframework.aop.aspectj.AspectJExpressionPointcut;
import org.springframework.aop.support.DefaultPointcutAdvisor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration;
import org.springframework.boot.autoconfigure.mongo.MongoAutoConfiguration;
import org.springframework.boot.autoconfigure.web.MultipartAutoConfiguration;
import org.springframework.context.ApplicationContext;
import org.springframework.context.ConfigurableApplicationContext;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.EnableAspectJAutoProxy;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.transaction.annotation.EnableTransactionManagement;
import org.springframework.transaction.interceptor.TransactionInterceptor;

@ComponentScan(basePackages={"com.wanshun"}/*,
        excludeFilters={
                @ComponentScan.Filter(type= FilterType.ASSIGNABLE_TYPE, value= RpcDriverStatsClientConfig.class),
                @ComponentScan.Filter(type= FilterType.ASSIGNABLE_TYPE, value= RpcDriverClientConfig.class)
        }*/)
@EnableScheduling
@EnableTransactionManagement
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class, MongoAutoConfiguration.class, MultipartAutoConfiguration.class})
@EnableAspectJAutoProxy(proxyTargetClass=true, exposeProxy = true)
public class PoolFileMain {
    private static final Logger logger = LoggerFactory.getLogger(PoolFileMain.class);

    public static void main(String[] args) {
        ConfigurableApplicationContext context = SpringApplication.run(PoolFileMain.class, args);
        execute(context);
        logger.info("任务已完成，退出应用程序");
        System.exit(0);
    }

    public static void execute(ApplicationContext context) {
        context.getBean(ExecuteService.class).execute();
    }

    @Bean
    public Advisor drivertatsAdvisor(@Autowired TransactionInterceptor txAdvice) {
        AspectJExpressionPointcut pointcut = new AspectJExpressionPointcut();
        pointcut.setExpression("execution(* com.wanshun.poolfile.service..*Impl.*(..))");
        return new DefaultPointcutAdvisor(pointcut, txAdvice);
    }

}
