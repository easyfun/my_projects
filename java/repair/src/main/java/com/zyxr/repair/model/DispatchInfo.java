package com.zyxr.repair.model;

import com.zyxr.product.model.asset.PhaseType;
import com.zyxr.repair.utils.Tool;

//标旧id,标题,借款本金,年化利率,加息利率,还款方式,期数类型,借款期数,标新id,借款人姓名,借款人旧uid,借款人新uid,还款期号,旧本金,新本金,本金差额,旧利息,新利息,利息差额,总差额
public class DispatchInfo {
	//标旧id
	public String oldAssetId;
	//标题
	public String title;
	//借款本金
	public String amount;
	//年化利率
	public String annualRate;
	//加息利率
	public String annualAddRate;
	//还款方式
	public int repayMode;
	//期数类型
	public PhaseType phaseType;
	//借款总期数
	public int phaseTotal;
	//标新id
	public long newAssetId;
	//借款人姓名
	public String borrowerName;
	//借款人旧uid
	public String oldBorrowerUid;
	//借款人新uid
	public long newBorrowerUid;
	//还款期号
	public long phase;
	//旧本金
	public String oldPrincipal;
	//新本金
	public long newPrincipal;
	//本金差额
	public long diffPrincipal;
	//旧利息
	public String oldInterest;
	//新利息
	public long newInterest;
	//利息差额
	public long diffInterest;
	//总差额
	public long diffTotal;
	
	
	public String getOldAssetId() {
		return oldAssetId;
	}
	public void setOldAssetId(String oldAssetId) {
		this.oldAssetId = oldAssetId;
	}
	public String getTitle() {
		return title;
	}
	public void setTitle(String title) {
		this.title = title;
	}
	public String getAmount() {
		return amount;
	}
	public void setAmount(String amount) {
		this.amount = amount;
	}
	public String getAnnualRate() {
		return annualRate;
	}
	public void setAnnualRate(String annualRate) {
		this.annualRate = annualRate;
	}
	public String getAnnualAddRate() {
		return annualAddRate;
	}
	public void setAnnualAddRate(String annualAddRate) {
		this.annualAddRate = annualAddRate;
	}
	public int getRepayMode() {
		return repayMode;
	}
	public void setRepayMode(int repayMode) {
		this.repayMode = repayMode;
	}
	public PhaseType getPhaseType() {
		return phaseType;
	}
	public void setPhaseType(PhaseType phaseType) {
		this.phaseType = phaseType;
	}
	public int getPhaseTotal() {
		return phaseTotal;
	}
	public void setPhaseTotal(int phaseTotal) {
		this.phaseTotal = phaseTotal;
	}
	public long getNewAssetId() {
		return newAssetId;
	}
	public void setNewAssetId(long newAssetId) {
		this.newAssetId = newAssetId;
	}
	public String getBorrowerName() {
		return borrowerName;
	}
	public void setBorrowerName(String borrowerName) {
		this.borrowerName = borrowerName;
	}
	public String getOldBorrowerUid() {
		return oldBorrowerUid;
	}
	public void setOldBorrowerUid(String oldBorrowerUid) {
		this.oldBorrowerUid = oldBorrowerUid;
	}
	public long getNewBorrowerUid() {
		return newBorrowerUid;
	}
	public void setNewBorrowerUid(long newBorrowerUid) {
		this.newBorrowerUid = newBorrowerUid;
	}
	public String getOldPrincipal() {
		return oldPrincipal;
	}
	public void setOldPrincipal(String oldPrincipal) {
		this.oldPrincipal = oldPrincipal;
	}
	public long getNewPrincipal() {
		return newPrincipal;
	}
	public void setNewPrincipal(long newPrincipal) {
		this.newPrincipal = newPrincipal;
	}
	public long getDiffPrincipal() {
		return diffPrincipal;
	}
	public void setDiffPrincipal(long diffPrincipal) {
		this.diffPrincipal = diffPrincipal;
	}
	public String getOldInterest() {
		return oldInterest;
	}
	public void setOldInterest(String oldInterest) {
		this.oldInterest = oldInterest;
	}
	public long getNewInterest() {
		return newInterest;
	}
	public void setNewInterest(long newInterest) {
		this.newInterest = newInterest;
	}
	public long getDiffInterest() {
		return diffInterest;
	}
	public void setDiffInterest(long diffInterest) {
		this.diffInterest = diffInterest;
	}
	public long getDiffTotal() {
		return diffTotal;
	}
	public void setDiffTotal(long diffTotal) {
		this.diffTotal = diffTotal;
	}
	public long getPhase() {
		return phase;
	}
	public void setPhase(long phase) {
		this.phase = phase;
	}
	@Override
	public String toString() {
		return "DispatchInfo [oldAssetId=" + oldAssetId + ", title=" + title + ", amount=" + amount + ", annualRate="
				+ annualRate + ", annualAddRate=" + annualAddRate + ", repayMode=" + repayMode + ", phaseType="
				+ phaseType + ", phaseTotal=" + phaseTotal + ", newAssetId=" + newAssetId + ", borrowerName="
				+ borrowerName + ", oldBorrowerUid=" + oldBorrowerUid + ", newBorrowerUid=" + newBorrowerUid
				+ ", phase=" + phase + ", oldPrincipal=" + oldPrincipal + ", newPrincipal=" + newPrincipal
				+ ", diffPrincipal=" + diffPrincipal + ", oldInterest=" + oldInterest + ", newInterest=" + newInterest
				+ ", diffInterest=" + diffInterest + ", diffTotal=" + diffTotal + "]";
	}

	public String getOut() {
		String out=oldAssetId+","+
				title+","+
				amount+","+
				annualRate+","+
				annualAddRate+","+
				repayMode+"["+Tool.getRepayModeDesc(repayMode)+"],"+
				phaseType.getValue()+","+
				phaseTotal+","+
				newAssetId+","+
				borrowerName+","+
				oldBorrowerUid+","+
				newBorrowerUid+","+
				phase+","+
				oldPrincipal+","+
				newPrincipal+","+
				diffPrincipal+","+
				oldInterest+","+
				newInterest+","+
				diffInterest+","+
				diffTotal;
		return out;
	}

	public String getOut2() {
		//标新id,借款人新uid,还款期号,总差额
		String out=newAssetId+","+
				newBorrowerUid+","+
				phase+","+
				diffTotal;
		return out;
	}

	public String getSumOut() {
		String out=borrowerName+","+
				oldBorrowerUid+","+
				newBorrowerUid+","+
				diffTotal;
		return out;		
	}
}
