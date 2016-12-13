 package com.zyxr.repair;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.commons.dbutils.DbUtils;
import org.apache.commons.dbutils.QueryRunner;
import org.apache.commons.dbutils.ResultSetHandler;
import org.apache.commons.dbutils.handlers.BeanListHandler;
import org.apache.ibatis.session.SqlSession;

import com.mysql.jdbc.Connection;
import com.zyxr.feecalculator.FeeConstant;
import com.zyxr.feecalculator.Repayment;
import com.zyxr.feecalculator.RepaymentSchedule;
import com.zyxr.repair.dao.DbManager;
import com.zyxr.repair.model.DispatchInfo;
import com.zyxr.repair.model.MissionLoans;
import com.zyxr.repair.model.MissionRepayment;
import com.zyxr.repair.utils.Tool;

public class RepaymentMissionLoan2 {
	public List<DispatchInfo> dispatchs;

	public List<String> assetIds;
	public List<String> checkAssetIds;
	

	public RepaymentMissionLoan2() {
		dispatchs=new ArrayList<DispatchInfo>();
		assetIds=new ArrayList<String>();
		checkAssetIds=new ArrayList<String>();
	}

	public void doJob() {
		File file=new File("D:\\job_projects\\java\\repair-workspace\\repair\\src\\main\\java\\com\\zyxr\\repair\\loan.list");
		InputStream is=null;
		StringBuffer fileContent=new StringBuffer();
		try {
			is = new FileInputStream(file);
			BufferedReader reader=new BufferedReader(new InputStreamReader(is));
			while (true) {
				String line=reader.readLine();
				if (null==line) {
					break;
				}
//				System.out.println("line="+line);
				
				String r=line.replace("\r", "");
				String l=r.replace("\n","");
//				System.out.println(l);
				assetIds.add(l);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		String url="jdbc:mysql://192.168.0.220:3306/toulf";
		String jdbcDriver="com.mysql.jdbc.Driver";
		String user="root";
		String password="123456";
		
		DbUtils.loadDriver(jdbcDriver);
		Connection conn=null;
		
		try {
			conn=(Connection) DriverManager.getConnection(url,user,password);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("连接MySQL失败,url="+url);
			Tool.exitSystem();
		}
		
		String sql="select id,title,status,borrower_id,borrower_name,amount,annual_rate,periods,repay_mode from mission_loans where status in (-1,0,1,2,3,4,5,6,7,8)";
		QueryRunner qr=new QueryRunner();
		ResultSetHandler<List<MissionLoans>> handler=new BeanListHandler<MissionLoans>(MissionLoans.class);
		List<MissionLoans> missionLoans=null;
		try {
			missionLoans = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}
		
		//System.out.println(missionLoans);
		
		Tool.initUidMap(missionLoans);
		
		for (MissionLoans missionLoan:missionLoans) {
			long newBorrowerUid=Tool.getNewUidByOldUid(missionLoan.borrower_id);
			long newAssetId=Tool.getMissionLoanAssetId(missionLoan.id, newBorrowerUid);
			String newAssetIdStr=String.valueOf(newAssetId);
			boolean isFound=false;
			for (String id:assetIds) {
				if (newAssetIdStr.equals(id)) {
					isFound=true;
					break;
				}
			}
			if (!isFound) {
				//System.out.println("不用考察,newAssetId="+newAssetId);
				continue;
			}
			
			isFound=false;
			for (String checkId: checkAssetIds) {
				if (newAssetIdStr.equals(checkId)) {
					isFound=true;
					break;
				}
			}
			if (!isFound) {
				checkAssetIds.add(newAssetIdStr);
			} else {
				System.out.println("重复标id还款");
				Tool.exitSystem();
			}
			
			
			long capital=Tool.getMoney(missionLoan.amount);
			long annualRate=Tool.getRate(missionLoan.annual_rate);
			long periods=Long.valueOf(missionLoan.periods);
			long periodType=FeeConstant.LOAN_MODE_DAY;
			String valueDate="2016-09-21 00:00:00";
			int repaymentMode=FeeConstant.REPAYMENT_MODE_XXHB;
			if (1!=missionLoan.repay_mode) {
				System.out.println("理财计划散标还款方式错误");
				Tool.exitSystem();
			}
			repaymentMode=Tool.getRepaymentMode(missionLoan.repay_mode);
			RepaymentSchedule rs=new RepaymentSchedule(capital,annualRate,periods,periodType,valueDate);
			rs.calRepaymentSchedule(repaymentMode);
			List<Repayment> repayments=rs.getRepaymentList();
			checkRepayments(conn,missionLoan,repayments);
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭MySQL连接失败");
			Tool.exitSystem();
		}
		
		saveFile();
		
		
//		System.out.println("已比较标");
		boolean isFound=false;
		List<String> notCheckAssetIds=new ArrayList<String>();
		for (String id:assetIds) {
			isFound=false;
			for (String id2:checkAssetIds) {
				if (id.equals(id2)) {
					System.out.println(id);
					isFound=true;
					break;
				}
			}
			if (!isFound) {
				notCheckAssetIds.add(id);
			}
		}
		
//		System.out.println("未比较");
//		System.out.println(notCheckAssetIds);
	}
	
	public void checkRepayments(Connection conn,MissionLoans missionLoan,List<Repayment> repayments) {
		QueryRunner qr=new QueryRunner();
		String sql=String.format("select id,loan_id,status,period,repay_mode,still_principal,still_interest,repay_date from mission_repayment where loan_id='%s'",
				missionLoan.id);
		ResultSetHandler<List<MissionRepayment>> handler=new BeanListHandler<MissionRepayment>(MissionRepayment.class);
		List<MissionRepayment> missionRepayments=null;
		try {
			missionRepayments = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}

		for (Repayment repayment:repayments) {
			for (MissionRepayment missionRepayment:missionRepayments) {
				if (/*missionRepayment.isRepaymented() ||*/ 
				repayment.getPeriodNo() != missionRepayment.period) {
					continue;
				}

				if (!Tool.checkRepaymentDate(missionRepayment.repay_date)) {
					continue;
				}

//				System.out.println(repayment.getRepaymentDateStr());
//				System.out.println(repayment.getPeriodNo());
				
				DispatchInfo dispatch=new DispatchInfo();
				long diffCapital=repayment.getCapital() - Tool.getMoney(missionRepayment.still_principal);
				long diffInterest=repayment.getInterest() - Tool.getMoney(missionRepayment.still_interest);
				
				if (diffCapital>10) {
					System.out.println("本金相差太大错误,diffCapital="+diffCapital);
					System.out.println("missionLoan="+missionLoan);
					System.out.println("repayment="+repayment);
					System.out.println("missionRepayment="+missionRepayment);
					Tool.exitSystem();
				}
				if (diffInterest>10) {
					System.out.println("利息相差太大错误,diffInterest="+diffInterest);
					System.out.println("missionLoan="+missionLoan);
					System.out.println("repayment="+repayment);
					System.out.println("missionRepayment="+missionRepayment);
					Tool.exitSystem();
				}

				long newBorrowerUid=Tool.getNewUidByOldUid(missionLoan.borrower_id);
				long newAssetId=Tool.getMissionLoanAssetId(missionLoan.id, newBorrowerUid);
				long phase=repayment.getPeriodNo();
				
				
				//标旧id
				dispatch.oldAssetId=missionLoan.id;
				//标题
				dispatch.title=missionLoan.title;
				//借款本金
				dispatch.amount=missionLoan.amount;
				//年化利率
				dispatch.annualRate=missionLoan.annual_rate;
				//加息利率
				dispatch.annualAddRate="0";
				//还款方式
				dispatch.repayMode=Tool.getRepaymentMode(missionLoan.repay_mode);
				//期数类型
				dispatch.phaseType=Tool.getPhaseType(1);
				//借款总期数
				dispatch.phaseTotal=missionLoan.periods;
				//标新id
				dispatch.newAssetId=newAssetId;
				//借款人姓名
				dispatch.borrowerName=missionLoan.borrower_name;
				//借款人旧uid
				dispatch.oldBorrowerUid=missionLoan.borrower_id;
				//借款人新uid
				dispatch.newBorrowerUid=Tool.getNewUidByOldUid(missionLoan.borrower_id);
				//还款期号
				dispatch.phase=phase;
				//旧本金
				dispatch.oldPrincipal=missionRepayment.still_principal;
				//新本金
				dispatch.newPrincipal=repayment.getCapital();
				//本金差额
				dispatch.diffPrincipal=diffCapital;
				//旧利息
				dispatch.oldInterest=missionRepayment.still_interest;
				//新利息
				dispatch.newInterest=repayment.getInterest();
				//利息差额
				dispatch.diffInterest=diffInterest;
				//总差额
				dispatch.diffTotal=diffCapital+diffInterest;
				
//				if (!Tool.checkNewRepayDate(dispatch)) {
//					continue;
//				}
				
				if (dispatch.diffTotal > 0) {
					dispatchs.add(dispatch);
				}
				break;
			}
		}
	}
	
	public void saveFile() {
		//标旧id,标题,借款本金,年化利率,加息利率,还款方式,期数类型,借款期数,标新id,借款人姓名,借款人旧uid,借款人新uid,还款期号,旧本金,新本金,本金差额,旧利息,新利息,利息差额,总差额
		StringBuffer fileDispatch=new StringBuffer();
		fileDispatch.append("标旧id,标题,借款本金,年化利率,加息利率,还款方式,期数类型,借款期数,标新id,借款人姓名,借款人旧uid,借款人新uid,还款期号,旧本金,新本金,本金差额,旧利息,新利息,利息差额,总差额");
		fileDispatch.append("\n");

		StringBuffer fileSimpleDispatch=new StringBuffer();
		fileSimpleDispatch.append("标新id,借款人新uid,还款期号,总差额");
		fileSimpleDispatch.append("\n");

		StringBuffer sumFileDispatch=new StringBuffer();
		sumFileDispatch.append("借款人姓名,借款人旧uid,借款人新uid,总差额");
		sumFileDispatch.append("\n");

		StringBuffer fileFixPrincipalSqls=new StringBuffer();
		StringBuffer fileFixInterestSqls=new StringBuffer();
		
		Map<String, DispatchInfo> sumDispatch=new HashMap<String, DispatchInfo>();
		
		for (DispatchInfo dispatch:dispatchs) {
			
			if (dispatch.diffPrincipal>0) {
				String fixSql=String.format("update product.t_repayments set expect_principal=%d+%d where asset_id=%d and borrower_uid=%d and phase=%d and state=0 and expect_principal=%d;",
						Tool.getMoney(dispatch.oldPrincipal),
						dispatch.diffPrincipal,
						dispatch.newAssetId,
						dispatch.newBorrowerUid,
						dispatch.phase,
						Tool.getMoney(dispatch.oldPrincipal));
				fileFixPrincipalSqls.append(fixSql);
				fileFixPrincipalSqls.append("\n");
				
				fixSql=String.format("update product.t_repayment_%02d set expect_principal=%d+%d where asset_id=%s and borrower_uid=%s and phase=%d and state=0 and expect_principal=%d;",
						dispatch.newBorrowerUid%100,
						Tool.getMoney(dispatch.oldPrincipal),
						dispatch.diffPrincipal,
						dispatch.newAssetId,
						dispatch.newBorrowerUid,
						dispatch.phase,
						Tool.getMoney(dispatch.oldPrincipal));
				fileFixPrincipalSqls.append(fixSql);
				fileFixPrincipalSqls.append("\n");
			}
		
			if (dispatch.diffInterest>0) {
				String fixSql=String.format("update product.t_repayments set expect_interest=%d+%d where asset_id=%d and borrower_uid=%d and phase=%d and state=0 and expect_interest=%d;",
						Tool.getMoney(dispatch.oldInterest),
						dispatch.diffInterest,
						dispatch.newAssetId,
						dispatch.newBorrowerUid,
						dispatch.phase,
						Tool.getMoney(dispatch.oldInterest));
				fileFixInterestSqls.append(fixSql);
				fileFixInterestSqls.append("\n");
				
				fixSql=String.format("update product.t_repayment_%02d set expect_interest=%d+%d where asset_id=%s and borrower_uid=%s and phase=%d and state=0 and expect_interest=%d;",
						dispatch.newBorrowerUid%100,
						Tool.getMoney(dispatch.oldInterest),
						dispatch.diffInterest,
						dispatch.newAssetId,
						dispatch.newBorrowerUid,
						dispatch.phase,
						Tool.getMoney(dispatch.oldInterest));
				fileFixInterestSqls.append(fixSql);
				fileFixInterestSqls.append("\n");
			}
			
			if (dispatch.diffTotal > 0) {
				fileDispatch.append(dispatch.getOut());
				fileDispatch.append("\n");
				
				fileSimpleDispatch.append(dispatch.getOut2());
				fileSimpleDispatch.append("\n");

				DispatchInfo dispatchInfo=sumDispatch.get(String.valueOf(dispatch.newBorrowerUid));
				if (null==dispatchInfo) {
					dispatchInfo=dispatch;
					sumDispatch.put(String.valueOf(dispatch.newBorrowerUid), dispatch);
				} else {
					dispatchInfo.diffTotal += dispatch.diffTotal;
				}
			}
		}

		
		try {
			if (fileDispatch.length() > 0) {
				File w=new File("D:\\job_projects\\python\\financial_plan_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-理财计划散标还款修复，D:\\job_projects\\python\\financial_plan_dispatch_money2.dat");
			Tool.exitSystem();
		}
		
		try {
			if (fileSimpleDispatch.length() > 0) {
				File w=new File("D:\\job_projects\\python\\financial_plan_simple_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileSimpleDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\financial_plan_simple_dispatch_money2.dat");
			Tool.exitSystem();
		}

		
		Iterator<Map.Entry<String, DispatchInfo>> iterator2=sumDispatch.entrySet().iterator();
		while (iterator2.hasNext()) {
			Map.Entry<String, DispatchInfo> entry=iterator2.next();
			DispatchInfo dispatchInfo=entry.getValue();
			sumFileDispatch.append(dispatchInfo.getSumOut());
			sumFileDispatch.append("\n");
		}

		try {
			if (sumFileDispatch.length() > 0) {
				File w=new File("D:\\job_projects\\python\\financial_plan_sum_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(sumFileDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-理财计划散标还款修复，D:\\job_projects\\python\\financial_plan_sum_dispatch_money2.dat");
			Tool.exitSystem();
		}


		try {
			if (fileFixPrincipalSqls.length() > 0) {
				File w=new File("D:\\job_projects\\python\\financial_plan_fix_principal_sqls2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileFixPrincipalSqls.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-理财计划散标还款修复，D:\\job_projects\\python\\financial_plan_fix_principal_sqls2.dat");
			Tool.exitSystem();
		}

		try {
			if (fileFixInterestSqls.length() > 0) {
				File w=new File("D:\\job_projects\\python\\financial_plan_fix_interest_sqls2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileFixInterestSqls.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-理财计划散标还款修复，D:\\job_projects\\python\\financial_plan_fix_interest_sqls2.dat");
			Tool.exitSystem();
		}
	}
	
	public void testMyBaits() {
        SqlSession session = DbManager.getSqlFactory().openSession();
        String statement = "com.zyxr.repair.missionLoansMap.getMissionLoans";
        List<MissionLoans> loans=session.selectList(statement);
        for (MissionLoans loan:loans) {
        	System.out.println(loan);
        }
        System.out.println(loans.size());
	}
	
}
