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
import java.util.Calendar;
import java.util.Date;
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
import com.zyxr.feecalculator.Repayment;
import com.zyxr.feecalculator.RepaymentSchedule;
import com.zyxr.repair.dao.DbManager;
import com.zyxr.repair.model.AssetInfo;
import com.zyxr.repair.model.DispatchInfo;
import com.zyxr.repair.model.LoanRepayment;
import com.zyxr.repair.model.Loans;
import com.zyxr.repair.model.MissionLoans;
import com.zyxr.repair.utils.Tool;

//已还的还款表修复
public class RepaymentedLoan2 {
	public List<DispatchInfo> dispatchs;
	
	public List<String> assetIds;
	public List<String> checkAssetIds;
	
	public RepaymentedLoan2() {
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
		
//		String sql="select id,title,status,borrower,borrowername,borrowamount,annualrate,add_rate,periods,periodmode,repaymode from loans where status in (6)";
		String sql="select id,title,status,borrower,borrowername,borrowamount,annualrate,add_rate,periods,periodmode,repaymode from loans where status in (1,2,3,4,5,6)";
		QueryRunner qr=new QueryRunner();
		ResultSetHandler<List<Loans>> handler=new BeanListHandler<Loans>(Loans.class);
		List<Loans> allLoans=null;
		try {
			allLoans = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}
		
		//System.out.println(loans);
		List<Loans> loans=new ArrayList<Loans>();
//		for (Loans loan : allLoans) {
//			
//		}
		loans.addAll(allLoans);
		
		Tool.initUidMapFromLoans(loans);
		
		for (Loans loan:loans) {
			long newBorrowerUid=Tool.getNewUidByOldUid(loan.borrower);
			long newAssetId=Tool.getLoanAssetId(loan.id, newBorrowerUid);
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

			
			long capital=Tool.getMoney(loan.borrowamount);
			long annualRate=Tool.getRate(loan.annualrate);
			long periods=Long.valueOf(loan.periods);
			long periodType=Tool.getPhaseType(loan.periodmode).getValue();
			String valueDate="2016-09-21 00:00:00";
			int repaymentMode=Tool.getRepaymentMode(loan.repaymode);
			repaymentMode=Tool.getRepaymentMode(loan.repaymode);
			RepaymentSchedule rs=new RepaymentSchedule(capital,annualRate,periods,periodType,valueDate);
			rs.calRepaymentSchedule(repaymentMode);
			List<Repayment> repayments=rs.getRepaymentList();
			checkRepayments(conn,loan,repayments);
		}
		
		try {
			conn.close();
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("关闭MySQL连接失败");
			Tool.exitSystem();
		}
		
		saveFile();
		
		System.out.println("已比较标");
		boolean isFound=false;
		List<String> notCheckAssetIds=new ArrayList<String>();
		for (String id:assetIds) {
			isFound=false;
			for (String id2:checkAssetIds) {
				if (id.equals(id2)) {
					//System.out.println(id);
					isFound=true;
					break;
				}
			}
			if (!isFound) {
				notCheckAssetIds.add(id);
			}
		}
		
		//System.out.println("未比较");
		//System.out.println(notCheckAssetIds);
	}
	
	public void checkRepayments(Connection conn,Loans loan,List<Repayment> repayments) {
		QueryRunner qr=new QueryRunner();
		String sql=String.format("select id,loansid,status,period,repaymode,stillPrincipal,stillInterest,repaydate from repayment where loansid='%s'",
				loan.id);
		ResultSetHandler<List<LoanRepayment>> handler=new BeanListHandler<LoanRepayment>(LoanRepayment.class);
		List<LoanRepayment> missionRepayments=null;
		try {
			missionRepayments = qr.query(conn, sql, handler);
		} catch (SQLException e) {
			e.printStackTrace();
			System.out.println("SQL查询错误,sql="+sql);
			Tool.exitSystem();
		}
		
		if (loan.id.equals("3137738379168973320388")) {
			System.out.println(loan.id);
		}

		//只看2016-11-03 23:59:59到2016-11-17 21:00:00之间还款
		for (Repayment repayment:repayments) {
			for (LoanRepayment missionRepayment:missionRepayments) {
				if (/*missionRepayment.isRepaymented() || */
					repayment.getPeriodNo() != missionRepayment.period) {
					continue;
				}
				
				if (!Tool.checkRepaymentDate(missionRepayment.repaydate)) {
					continue;
				}
				
				DispatchInfo dispatch=new DispatchInfo();
				long diffCapital=repayment.getCapital() - Tool.getMoney(missionRepayment.stillPrincipal);
				long diffInterest=repayment.getInterest() - Tool.getMoney(missionRepayment.stillInterest);
				
				if (diffCapital>10) {
					System.out.println("本金相差太大错误,diffCapital="+diffCapital);
					System.out.println("missionLoan="+loan);
					System.out.println("repayment="+repayment);
					System.out.println("missionRepayment="+missionRepayment);
					Tool.exitSystem();
				}
				if (diffInterest>10) {
					System.out.println("利息相差太大错误,diffInterest="+diffInterest);
					System.out.println("missionLoan="+loan);
					System.out.println("repayment="+repayment);
					System.out.println("missionRepayment="+missionRepayment);
					Tool.exitSystem();
				}

				long newBorrowerUid=Tool.getNewUidByOldUid(loan.borrower);
				long newAssetId=Tool.getLoanAssetId(loan.id, newBorrowerUid);
				long phase=repayment.getPeriodNo();
				
				
				//标旧id
				dispatch.oldAssetId=loan.id;
				//标题
				dispatch.title=loan.title;
				//借款本金
				dispatch.amount=loan.borrowamount;
				//年化利率
				dispatch.annualRate=loan.annualrate;
				//加息利率
				dispatch.annualAddRate=loan.add_rate;
				//还款方式
				dispatch.repayMode=Tool.getRepaymentMode(loan.repaymode);
				//期数类型
				dispatch.phaseType=Tool.getPhaseType(loan.periodmode);
				//借款总期数
				dispatch.phaseTotal=loan.periods;
				//标新id
				dispatch.newAssetId=newAssetId;
				//借款人姓名
				dispatch.borrowerName=loan.borrowername;
				//借款人旧uid
				dispatch.oldBorrowerUid=loan.borrower;
				//借款人新uid
				dispatch.newBorrowerUid=Tool.getNewUidByOldUid(loan.borrower);
				//还款期号
				dispatch.phase=phase;
				//旧本金
				dispatch.oldPrincipal=missionRepayment.stillPrincipal;
				//新本金
				dispatch.newPrincipal=repayment.getCapital();
				//本金差额
				dispatch.diffPrincipal=diffCapital;
				//旧利息
				dispatch.oldInterest=missionRepayment.stillInterest;
				//新利息
				dispatch.newInterest=repayment.getInterest();
				//利息差额
				dispatch.diffInterest=diffInterest;
				//总差额
				dispatch.diffTotal=diffCapital+diffInterest;
				
/*				if (Tool.isNewRepayed(dispatch)) {
					continue;
				}*/
//				if (!Tool.checkNewRepayDate(dispatch)) {
//					continue;
//				}
				
				if (dispatch.diffTotal > 0) {
					dispatchs.add(dispatch);
					
					if (loan.id.equals("3137738379168973320388")) {
						System.out.println(dispatch);
					}

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
		
		StringBuffer sumFileDispatch=new StringBuffer();
		sumFileDispatch.append("借款人姓名,借款人旧uid,借款人新uid,总差额");
		sumFileDispatch.append("\n");
		
		StringBuffer fileSimpleDispatch=new StringBuffer();
		fileSimpleDispatch.append("标新id,借款人新uid,还款期号,总差额");
		fileSimpleDispatch.append("\n");

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
				File w=new File("D:\\job_projects\\python\\asset_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\asset_dispatch_money2.dat");
			Tool.exitSystem();
		}

		try {
			if (fileSimpleDispatch.length() > 0) {
				File w=new File("D:\\job_projects\\python\\asset_simple_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileSimpleDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\asset_simple_dispatch_money2.dat");
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
				File w=new File("D:\\job_projects\\python\\asset_sum_dispatch_money2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(sumFileDispatch.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\asset_sum_dispatch_money2.dat");
			Tool.exitSystem();
		}

		try {
			if (fileFixPrincipalSqls.length() > 0) {
				File w=new File("D:\\job_projects\\python\\asset_fix_principal_sqls2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileFixPrincipalSqls.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\asset_fix_principal_sqls2.dat");
			Tool.exitSystem();
		}

		try {
			if (fileFixInterestSqls.length() > 0) {
				File w=new File("D:\\job_projects\\python\\asset_fix_interest_sqls2.dat");
				   FileWriter fw = new FileWriter(w.getAbsoluteFile());
				   BufferedWriter bw = new BufferedWriter(fw);
				   bw.write(fileFixInterestSqls.toString());
				   bw.close();			
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println("保存文件失败-散标还款修复，D:\\job_projects\\python\\asset_fix_interest_sqls2.dat");
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
