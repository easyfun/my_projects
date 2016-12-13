package com.financial.money;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.DbUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.ResultSetHandler;
import org.apache.commons.dbutils.handlers.BeanListHandler;

import com.financial.money.model.PayoffInfo;
import com.financial.money.model.Repayment;
import com.mysql.jdbc.Connection;
import com.zyxr.repair.utils.Tool;

public class FinancialMoney {
	public long payoffTotalAmount=0;
	public Map<String, String> investorUids;
	
	public FinancialMoney() {
		investorUids=new HashMap<String, String>();
	}

	public void doJob() {
		String url = "jdbc:mysql://192.168.51.145:3306/product";
		String jdbcDriver = "com.mysql.jdbc.Driver";
		String user = "search";
		String password = "search@zyxr.com";

		DbUtils.loadDriver(jdbcDriver);
		Connection conn = null;

		try {
			conn = (Connection) DriverManager.getConnection(url, user, password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,url=" + url);
			Tool.exitSystem();
		}

		StringBuffer content = new StringBuffer();

		for (int i=0;i<100;i++) {
			String sql = String.format("select repayment_id,borrower_uid,asset_id,asset_type,asset_pool,"
					+ "asset_name,phase_total,phase_mode,phase,actual_principal,actual_interest,"
					+ "actual_penalty,actual_compensation,actual_pay_guarantee,actual_pay_incoming,"
					+ "actual_pay_platform,actual_pay_fee,actual_date from product.t_repayment_%02d "
					+ "where state=3 and asset_type=0 and asset_pool=2 and "
					+ "actual_date<='2016-11-17 20:00:00' and actual_date>='2016-11-04 00:00:00'",
					i);
			QueryRunner qr = new QueryRunner();
			ResultSetHandler<List<Repayment>> handler = new BeanListHandler<Repayment>(Repayment.class);
			List<Repayment> repayments = null;
			try {
				repayments = qr.query(conn, sql, handler);
			} catch (SQLException e) {
				e.printStackTrace();
				System.out.println("SQL查询错误,sql=" + sql);
				Tool.exitSystem();
			}
	
			for (Repayment repayment : repayments) {
				content.append(repayment.title());
				content.append(repayment.getOut());
				checkPayoffInfo(content, conn, repayment);
			}
			content.append("总还款金额:"+payoffTotalAmount+"\n");
		}
		
		if (content.length()>0) {
			try {
				File w = new File("D:\\job_projects\\python\\financial_plan_money.dat");
				FileWriter fw = new FileWriter(w.getAbsoluteFile());
				BufferedWriter bw = new BufferedWriter(fw);
				bw.write(content.toString());
				bw.close();
			} catch (IOException e1) {
				e1.printStackTrace();
				System.out.println("保存文件错误");
			}
		}

		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭MySQL连接失败");
			Tool.exitSystem();
		}
		
		StringBuffer uidContent=new StringBuffer();
		Iterator<Map.Entry<String, String>> iterator=investorUids.entrySet().iterator();
		while (iterator.hasNext()) {
			Map.Entry<String, String> entry=iterator.next();
			String investorUid=entry.getKey();
			uidContent.append(investorUid);
			uidContent.append("\n");
		}
		if (uidContent.length()>0) {
			try {
				File w = new File("D:\\job_projects\\python\\financial_plan_get_money_uids.dat");
				FileWriter fw = new FileWriter(w.getAbsoluteFile());
				BufferedWriter bw = new BufferedWriter(fw);
				bw.write(uidContent.toString());
				bw.close();
			} catch (IOException e1) {
				e1.printStackTrace();
				System.out.println("保存文件错误");
			}
		}
		
	}

	public void checkPayoffInfo(StringBuffer content, Connection conn, Repayment repayment) {
		long totalAmount=0;
		for (int i=0;i<100;i++) {
			String sql = String.format(
					"select payoff_id,asset_id,asset_name,asset_type,asset_pool"
					+ ",investor_uid,borrower_uid,repayment_id," 
					+ "phase,actual_principal,actual_interest,"
					+ "actual_add_interest,actual_pay_platform," 
					+ "actual_date,state,property "
					+ "from invest.t_investment_payoff_%02d where state=3 and asset_id=%d and "
					+ " repayment_id=%d and borrower_uid=%d",
					i, repayment.asset_id, repayment.repayment_id, repayment.borrower_uid);
//			System.out.println("payoff sql="+sql);
			QueryRunner qr = new QueryRunner();
			ResultSetHandler<List<PayoffInfo>> handler = new BeanListHandler<PayoffInfo>(PayoffInfo.class);
			List<PayoffInfo> payoffInfos = null;
			try {
				payoffInfos = qr.query(conn, sql, handler);
			} catch (SQLException e) {
				e.printStackTrace();
				System.out.println("SQL查询错误,sql=" + sql);
				Tool.exitSystem();
			}
	
			if (payoffInfos.size() <= 0) {
//				System.out.println("回款表空");
				continue;
			}
	
			for (PayoffInfo payoffInfo : payoffInfos) {
				content.append(payoffInfo.title());
				content.append(payoffInfo.getOut());
				
				totalAmount+=payoffInfo.actual_principal+payoffInfo.actual_interest;
				
//				checkInvestInfo(conn, repayment, payoffInfo);
				String uid=String.valueOf(payoffInfo.investor_uid);
				investorUids.put(uid, uid);
			}
		}
		content.append("本期回款总金额:"+totalAmount+",差额="+(repayment.actual_principal+repayment.actual_interest-totalAmount)+"\n");
		payoffTotalAmount+=totalAmount;
	}
	
	public void checkInvestInfo(Connection conn, Repayment repayment, PayoffInfo payoffInfo) {
		
	}
}
