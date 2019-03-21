package {{project_package_name}}.springboot;

import {{project_package_name}}.constants.{{project_module_constant}};
import com.alibaba.fastjson.parser.ParserConfig;
import com.wanshun.common.web.WsTomcatEmbeddedServletContainerFactory;
import com.weibo.api.motan.common.MotanConstants;
import com.weibo.api.motan.util.MotanSwitcherUtil;
import org.springframework.aop.Advisor;
import org.springframework.aop.aspectj.AspectJExpressionPointcut;
import org.springframework.aop.support.DefaultPointcutAdvisor;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.jdbc.DataSourceAutoConfiguration;
import org.springframework.boot.autoconfigure.mongo.MongoAutoConfiguration;
import org.springframework.boot.autoconfigure.web.MultipartAutoConfiguration;
import org.springframework.boot.context.embedded.tomcat.TomcatEmbeddedServletContainerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.context.annotation.EnableAspectJAutoProxy;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.transaction.annotation.EnableTransactionManagement;
import org.springframework.transaction.interceptor.TransactionInterceptor;
import org.springframework.context.annotation.FilterType;

/**
 * 父类配置了spring boot相关注解
 *
 */
@ComponentScan(basePackages={"com.wanshun"},
		excludeFilters={
				@ComponentScan.Filter(type= FilterType.ASSIGNABLE_TYPE, value= {{project_rpc_client_config}}.class),
		})
@EnableScheduling
@EnableTransactionManagement
@SpringBootApplication(exclude = {DataSourceAutoConfiguration.class, MongoAutoConfiguration.class, MultipartAutoConfiguration.class})
@EnableAspectJAutoProxy(proxyTargetClass=true, exposeProxy = true)
public class {{project_impl_main_class}} {

	public static void main(String[] args) {
		SpringApplication.run({{project_impl_main_class}}.class, args);

		ParserConfig.getGlobalInstance().setAutoTypeSupport(true);

		//motan rpc初始化完成
		MotanSwitcherUtil.setSwitcherValue(MotanConstants.REGISTRY_HEARTBEAT_SWITCHER, true);
	}
	
	@Bean
	public TomcatEmbeddedServletContainerFactory tomcatEmbeddedServletContainerFactory() throws Exception{
		TomcatEmbeddedServletContainerFactory bean = new WsTomcatEmbeddedServletContainerFactory({{project_http_port}}, {{project_https_port}});
		return bean;
	}

	
	@Bean
	public Advisor drivertatsAdvisor(@Autowired TransactionInterceptor txAdvice) {
		AspectJExpressionPointcut pointcut = new AspectJExpressionPointcut();
		pointcut.setExpression("execution(* {{project_package_name}}.service..*Impl.*(..))");
		return new DefaultPointcutAdvisor(pointcut, txAdvice);
	}
}
