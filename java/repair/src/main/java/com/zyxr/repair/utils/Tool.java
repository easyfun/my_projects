package com.zyxr.repair.utils;

import java.math.BigDecimal;
import java.rmi.server.UID;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Calendar;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.DbUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.ResultSetHandler;
import org.apache.commons.dbutils.handlers.BeanListHandler;

import com.mysql.jdbc.Connection;
import com.zyxr.feecalculator.FeeConstant;
import com.zyxr.product.model.asset.PhaseType;
import com.zyxr.repair.model.DispatchInfo;
import com.zyxr.repair.model.Loans;
import com.zyxr.repair.model.MissionLoans;
import com.zyxr.repair.model.NewRepayment;
import com.zyxr.repair.model.NewRepaymentDate;
import com.zyxr.repair.model.UidInfo;

public class Tool {
	public static BigDecimal bd100=new BigDecimal(100);
	public static Map<String, Long> uid2UidFromMissionLoan=new HashMap<String, Long>();
	public static Map<String, Long> uid2UidFromLoan=new HashMap<String, Long>();
	
	public static long getMoney(String amount) {
		BigDecimal bdAmount=new BigDecimal(amount);
		return bdAmount.multiply(bd100).longValue();
	}

	public static long getRate(String rate) {
		BigDecimal bdRate=new BigDecimal(rate);
		return bdRate.multiply(bd100).longValue();
	}
	
	public static int getRepaymentMode(int oldMode) {
		//还款方式：1先息后本 2等额平息 3等额本息 4等额本金 5一次性
		int repaymentMode=-1;
		switch (oldMode) {
		case 1:
			repaymentMode=FeeConstant.REPAYMENT_MODE_XXHB;
			break;
		case 2:
			repaymentMode=FeeConstant.REPAYMENT_MODE_DEPX;
			break;
		case 3:
			repaymentMode=FeeConstant.REPAYMENT_MODE_DEBX;
			break;
		case 4:
			repaymentMode=FeeConstant.REPAYMENT_MODE_DEBJ;
			break;
		case 5:
			repaymentMode=FeeConstant.REPAYMENT_MODE_YCBX;
			break;
		default:
			System.out.println("还款方式错误,oldMode="+oldMode);
			exitSystem();
			break;
		}
		return repaymentMode;
	}
	
	public static String getRepayModeDesc(int mode) {
		String desc="";
		if (mode==FeeConstant.REPAYMENT_MODE_XXHB) {
			desc="先息后本";
		} else if (mode==FeeConstant.REPAYMENT_MODE_DEPX) {
			desc="等额平息";
		} else if (mode==FeeConstant.REPAYMENT_MODE_DEBX) {
			desc="等额本息";
		} else if (mode==FeeConstant.REPAYMENT_MODE_DEBJ) {
			desc="等额本金";
		} else if (mode==FeeConstant.REPAYMENT_MODE_YCBX) {
			desc ="一次本息";
		} else {
			System.out.println("还款方式错误,oldMode="+mode);
			exitSystem();
		}
		return desc;
	}
	

	
	public static PhaseType getPhaseType(int periodmode) {
		PhaseType phaseType=null;
		switch (periodmode) {//借款期数类型 0月 1天
		case 0:
			phaseType=PhaseType.ONEMONTH_PHASE;
			break;
		case 1:
			phaseType=PhaseType.ONEDAY_PHASE;
			break;
		default:
			System.out.println("借款期数类型错误");
			exitSystem();
			break;
		}
		return phaseType;
	}

	public static long getNewUidByOldUid(String oldUid) {
		Long newUid=uid2UidFromMissionLoan.get(oldUid);
		if (null!=newUid) {
			return newUid;
		}
			
		newUid=uid2UidFromLoan.get(oldUid);
		if (null!=newUid) {
			return newUid;
		}
			
		System.out.println("获取新uid失败,oldUid="+oldUid);
		exitSystem();
		return -1;
	}
	
	public static long getMissionLoanAssetId(String loanid, long newUid) {
		long assetId=0;
		int len=loanid.length()-13;//22-8
		
		String newBorrowerUid=String.valueOf(newUid);
		int len1=newBorrowerUid.length()-2;
		
		String asset_id22 = "3" + loanid.substring(len) +newBorrowerUid.substring(len1);
		assetId=Long.valueOf(asset_id22);
		return assetId;
	}
	
	public static long getLoanAssetId(String loanid, long newUid) {
		long assetId=0;
		int len=loanid.length()-16;

		String newBorrowerUid=String.valueOf(newUid);
		int len1=newBorrowerUid.length()-2;
		
		String asset_id = loanid.substring(len) + newBorrowerUid.substring(len1);
		assetId=Long.valueOf(asset_id);
		return assetId;
	}
	
	public static void exitSystem() {
		System.exit(-1);
	}

	public static void init() {
//		initUidMap();
	}
	
	public static void initUidMap(List<MissionLoans> missionLoans) {
		if (uid2UidFromMissionLoan.size()>0) {
			return;
		}
		
		String url="jdbc:mysql://192.168.51.145:3306/user";
		String jdbcDriver="com.mysql.jdbc.Driver";
		String user="search";
		String password="search@zyxr.com";
		
		DbUtils.loadDriver(jdbcDriver);
		Connection conn=null;
		
		try {
			conn=(Connection) DriverManager.getConnection(url,user,password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,"+url);
			Tool.exitSystem();
		}
		
		for (MissionLoans missionLoan:missionLoans) {
			Long newUid=uid2UidFromMissionLoan.get(missionLoan.borrower_id);
			if (null!=newUid) {
				continue;
			}

			String tablename = "user.t_userid_rel_olduid";
			String sql = "select userid, ybuid from " + tablename + " where ybuid='"+missionLoan.borrower_id+"'";
			//System.out.println("sql="+sql);
			QueryRunner qr=new QueryRunner();
			ResultSetHandler<List<UidInfo>> handler=new BeanListHandler<UidInfo>(UidInfo.class);
			List<UidInfo> uids=null;
			try {
				uids = qr.query(conn, sql, handler);
			} catch (SQLException e) {
				e.printStackTrace();
				System.out.println("SQL查询错误,sql="+sql);
				Tool.exitSystem();
			}
	
			for (UidInfo uid:uids){
				uid2UidFromMissionLoan.put(uid.ybuid, uid.userid);
			}
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭数据库连接失败,url="+url);
			exitSystem();
		}
	}
	
	public static void initUidMapFromLoans(List<Loans> loans) {
		if (uid2UidFromLoan.size()>0) {
			return;
		}
		
		String url="jdbc:mysql://192.168.51.145:3306/user";
		String jdbcDriver="com.mysql.jdbc.Driver";
		String user="search";
		String password="search@zyxr.com";
		
		DbUtils.loadDriver(jdbcDriver);
		Connection conn=null;
		
		try {
			conn=(Connection) DriverManager.getConnection(url,user,password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,"+url);
			Tool.exitSystem();
		}
		
		for (Loans loan:loans) {
			Long newUid=uid2UidFromLoan.get(loan.borrower);
			if (null!=newUid) {
				continue;
			}
			
			String tablename = "user.t_userid_rel_olduid";
			String sql = "select userid, ybuid from " + tablename + " where ybuid='"+loan.borrower+"'";
			//System.out.println("sql="+sql);
			QueryRunner qr=new QueryRunner();
			ResultSetHandler<List<UidInfo>> handler=new BeanListHandler<UidInfo>(UidInfo.class);
			List<UidInfo> uids=null;
			try {
				uids = qr.query(conn, sql, handler);
			} catch (SQLException e) {
				e.printStackTrace();
				System.out.println("SQL查询错误,sql="+sql);
				Tool.exitSystem();
			}
	
			for (UidInfo uid:uids){
				uid2UidFromLoan.put(uid.ybuid, uid.userid);
			}
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭数据库连接失败,url="+url);
			exitSystem();
		}
	}
	
	public static boolean isNewRepayed(DispatchInfo dispatchInfo) {
		boolean isRepayed=false;
		
		String url="jdbc:mysql://192.168.51.145:3306/user";
		String jdbcDriver="com.mysql.jdbc.Driver";
		String user="search";
		String password="search@zyxr.com";
		
		DbUtils.loadDriver(jdbcDriver);
		Connection conn=null;
		
		try {
			conn=(Connection) DriverManager.getConnection(url,user,password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,"+url);
			Tool.exitSystem();
		}
		
		String tablename = String.format("product.t_repayment_%02d",dispatchInfo.newBorrowerUid%100);
		String sql = String.format("select state from %s where asset_id=%d and borrower_uid=%d and phase=%d",
				tablename,
				dispatchInfo.newAssetId,
				dispatchInfo.newBorrowerUid,
				dispatchInfo.phase);
		//System.out.println("sql="+sql);
		QueryRunner qr=new QueryRunner();
		ResultSetHandler<List<NewRepayment>> handler=new BeanListHandler<NewRepayment>(NewRepayment.class);
		List<NewRepayment> statuses=null;
		
		try {
			statuses = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}

		System.out.println(statuses);
		for (NewRepayment status:statuses){
			if (0==status.state) {
				isRepayed=false;
				break;
			} else {
				isRepayed=true;
			}
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭数据库连接失败,url="+url);
			exitSystem();
		}
		
		return isRepayed;
	}
	
	public static boolean checkNewRepayDate(DispatchInfo dispatchInfo) {
		boolean isRepayed=false;
		
		String url="jdbc:mysql://192.168.51.145:3306/user";
		String jdbcDriver="com.mysql.jdbc.Driver";
		String user="search";
		String password="search@zyxr.com";
		
		DbUtils.loadDriver(jdbcDriver);
		Connection conn=null;
		
		try {
			conn=(Connection) DriverManager.getConnection(url,user,password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,"+url);
			Tool.exitSystem();
		}
		
		String tablename = String.format("product.t_repayment_%02d",dispatchInfo.newBorrowerUid%100);
		String sql = String.format("select expect_date from %s where asset_id=%d and borrower_uid=%d and phase=%d",
				tablename,
				dispatchInfo.newAssetId,
				dispatchInfo.newBorrowerUid,
				dispatchInfo.phase);
		//System.out.println("sql="+sql);
		QueryRunner qr=new QueryRunner();
		ResultSetHandler<List<NewRepaymentDate>> handler=new BeanListHandler<NewRepaymentDate>(NewRepaymentDate.class);
		List<NewRepaymentDate> dates=null;
		
		try {
			dates = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}

		System.out.println(dates);
		for (NewRepaymentDate date:dates){
			
			isRepayed=Tool.checkRepaymentDate(date.expect_date);
			break;
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭数据库连接失败,url="+url);
			exitSystem();
		}
		
		return isRepayed;
	}
	
	public static boolean checkRepaymentDate(Date repayDate) {
//		if (null==repayDate) {
//			return false;
//		}
		Calendar begin=Calendar.getInstance();
		begin.set(2016, 10, 4, 0, 0, 0);
		
		Calendar end=Calendar.getInstance();
		end.set(2016,10,21,21,0,0);

		Calendar repay=Calendar.getInstance();
		repay.setTime(repayDate);
		
		if (begin.getTimeInMillis()<=repay.getTimeInMillis() 
				&& repay.getTimeInMillis()<=end.getTimeInMillis()) {
			return true;
		}
		return false;
	}

}
