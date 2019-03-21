package com.wanshun.poolfile.service.switchds.driver.impl;

import com.wanshun.poolfile.service.switchds.driver.DriverService;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Service;

@Service
public class DriverServiceImpl implements DriverService {
    private static final Logger logger = LoggerFactory.getLogger(DriverServiceImpl.class);

    @Override
    public void selectList() {
        logger.info("selectList");
    }
}
