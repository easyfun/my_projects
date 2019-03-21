package com.wanshun.poolfile.springboot.config.datamodel;

import org.aspectj.lang.ProceedingJoinPoint;
import org.aspectj.lang.annotation.Around;
import org.aspectj.lang.annotation.Aspect;
import org.aspectj.lang.reflect.MethodSignature;
import org.springframework.core.annotation.Order;
import org.springframework.stereotype.Component;

import java.lang.reflect.Method;

@Aspect
@Order(1)
@Component
public class DynamicDataSourceAspect {

    /**
     * 数据源优先级
     * 1.方法上的TargetDataSource注解
     * 2.类上的TargetDataSource注解
     * 3.默认数据源driverStats
     * */
    @Around("execution(* com.wanshun.fileexport.service.switchds..*Impl.*(..))")
    public Object switchDS(ProceedingJoinPoint point) throws Throwable {
        Class<?> className = point.getTarget().getClass();

        DataSourceKey dataSource = DataSourceKey.fileExport;

        // 得到访问的方法对象
        String methodName = point.getSignature().getName();
        Class[] argClass = ((MethodSignature) point.getSignature()).getParameterTypes();
        Method method = className.getMethod(methodName, argClass);
        // 判断是否存在@TargetDataSource注解
        if (method.isAnnotationPresent(TargetDataSource.class)) {
            TargetDataSource annotation = method.getAnnotation(TargetDataSource.class);
            // 取出注解中的数据源名
            dataSource = annotation.value();
        } else if (className.isAnnotationPresent(TargetDataSource.class)) {
            TargetDataSource ds = className.getAnnotation(TargetDataSource.class);
            dataSource = ds.value();
        }

        // 切换数据源
        DataSourceContextHolder.setDB(dataSource);

        try {
            return point.proceed();
        } finally {
            DataSourceContextHolder.clearDB();
        }
    }
}