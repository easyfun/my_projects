package com.wanshun.fileexport.springboot.config;

import com.wanshun.common.code.FilterSortNum;
import com.wanshun.fileexport.springboot.filter.AuthFilter;
import org.springframework.boot.web.servlet.FilterRegistrationBean;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class WebConfig {
    
	/**
	 * 过滤器注册
	 */
	@Bean
	public FilterRegistrationBean springBootActuatorFilter() {
        FilterRegistrationBean bean = new FilterRegistrationBean();
        bean.setFilter(new AuthFilter());
        bean.addUrlPatterns("/*");
        bean.setOrder(FilterSortNum.ACTUATOR_FILTER);//设置过滤器序号
        return bean;
    }
}
