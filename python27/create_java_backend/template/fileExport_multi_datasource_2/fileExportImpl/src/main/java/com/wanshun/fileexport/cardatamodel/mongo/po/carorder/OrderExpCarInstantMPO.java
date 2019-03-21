package com.wanshun.fileexport.cardatamodel.mongo.po.carorder;

import com.wanshun.common.database.mongo.MorphiaBasePo;
import org.bson.types.ObjectId;
import org.mongodb.morphia.annotations.Entity;
import org.mongodb.morphia.annotations.Id;

import java.util.Date;

@Entity(value="OrderExpCarInstant", noClassnameStored=true)
public class OrderExpCarInstantMPO extends MorphiaBasePo {

	private static final long serialVersionUID = -2353232194552018886L;
	
	@Id
    private ObjectId mongoId;

	private String orderId;

	private Integer dataNodeId;

	private Integer channel;

	private Long appUserId;

	private Long driverId;

	private String category;

	private Date receivedOrderTime;

	private Date driverArriveTime;

	private Date startChargeTime;

	private Date endChargeTime;

	private Date confirmPayTime;

	private Date payTime;

	private Integer orderPrice;

	private Integer discountPrice;

	private String surcharge;

	private Integer realPayPrice;

	private Double platfromChargeRatio;

	private Integer platformChargeMoney;

	private Integer driverIncome;

	private Integer paidMoney;

	private Integer repairMoney;

	private Integer balanceMoney;

	private Integer status;

	private Integer isUserDelete;

	private Integer callForOther;

	private Integer cityCode;

	private Integer areaCode;

	private Integer cancelReason;

	private String cancelReasonMark;

	private Integer level;

	private Integer orgType;

	private Integer relationId;

	private Integer payType;

	private Integer dispatchCategory;

	private Integer isAsync;

	private Integer orderScore;

	private Integer smsWarn;

	private Integer mileage;

	private String orderDetail;

	private String userComment;

	private String driverComment;

	private Double driverScore;

	private Integer passengerDel;

	private Integer driverDel;

	private Double userScore;

	private String passengerPhone;

	private String agencyNumber;

	private Date updateTime;

	private Integer partitionId;

	private Integer statement;

	private Integer checkStatus;

	private Date createTime;

	private String dataSource;

	public ObjectId getMongoId() {
		return mongoId;
	}

	public void setMongoId(ObjectId mongoId) {
		this.mongoId = mongoId;
	}

	public String getOrderId() {
		return orderId;
	}

	public void setOrderId(String orderId) {
		this.orderId = orderId;
	}

	public Integer getDataNodeId() {
		return dataNodeId;
	}

	public void setDataNodeId(Integer dataNodeId) {
		this.dataNodeId = dataNodeId;
	}

	public Integer getChannel() {
		return channel;
	}

	public void setChannel(Integer channel) {
		this.channel = channel;
	}

	public Long getAppUserId() {
		return appUserId;
	}

	public void setAppUserId(Long appUserId) {
		this.appUserId = appUserId;
	}

	public Long getDriverId() {
		return driverId;
	}

	public void setDriverId(Long driverId) {
		this.driverId = driverId;
	}

	public String getCategory() {
		return category;
	}

	public void setCategory(String category) {
		this.category = category;
	}

	public Date getReceivedOrderTime() {
		return receivedOrderTime;
	}

	public void setReceivedOrderTime(Date receivedOrderTime) {
		this.receivedOrderTime = receivedOrderTime;
	}

	public Date getDriverArriveTime() {
		return driverArriveTime;
	}

	public void setDriverArriveTime(Date driverArriveTime) {
		this.driverArriveTime = driverArriveTime;
	}

	public Date getStartChargeTime() {
		return startChargeTime;
	}

	public void setStartChargeTime(Date startChargeTime) {
		this.startChargeTime = startChargeTime;
	}

	public Date getEndChargeTime() {
		return endChargeTime;
	}

	public void setEndChargeTime(Date endChargeTime) {
		this.endChargeTime = endChargeTime;
	}

	public Date getConfirmPayTime() {
		return confirmPayTime;
	}

	public void setConfirmPayTime(Date confirmPayTime) {
		this.confirmPayTime = confirmPayTime;
	}

	public Date getPayTime() {
		return payTime;
	}

	public void setPayTime(Date payTime) {
		this.payTime = payTime;
	}

	public Integer getOrderPrice() {
		return orderPrice;
	}

	public void setOrderPrice(Integer orderPrice) {
		this.orderPrice = orderPrice;
	}

	public Integer getDiscountPrice() {
		return discountPrice;
	}

	public void setDiscountPrice(Integer discountPrice) {
		this.discountPrice = discountPrice;
	}

	public String getSurcharge() {
		return surcharge;
	}

	public void setSurcharge(String surcharge) {
		this.surcharge = surcharge;
	}

	public Integer getRealPayPrice() {
		return realPayPrice;
	}

	public void setRealPayPrice(Integer realPayPrice) {
		this.realPayPrice = realPayPrice;
	}

	public Double getPlatfromChargeRatio() {
		return platfromChargeRatio;
	}

	public void setPlatfromChargeRatio(Double platfromChargeRatio) {
		this.platfromChargeRatio = platfromChargeRatio;
	}

	public Integer getPlatformChargeMoney() {
		return platformChargeMoney;
	}

	public void setPlatformChargeMoney(Integer platformChargeMoney) {
		this.platformChargeMoney = platformChargeMoney;
	}

	public Integer getDriverIncome() {
		return driverIncome;
	}

	public void setDriverIncome(Integer driverIncome) {
		this.driverIncome = driverIncome;
	}

	public Integer getPaidMoney() {
		return paidMoney;
	}

	public void setPaidMoney(Integer paidMoney) {
		this.paidMoney = paidMoney;
	}

	public Integer getRepairMoney() {
		return repairMoney;
	}

	public void setRepairMoney(Integer repairMoney) {
		this.repairMoney = repairMoney;
	}

	public Integer getBalanceMoney() {
		return balanceMoney;
	}

	public void setBalanceMoney(Integer balanceMoney) {
		this.balanceMoney = balanceMoney;
	}

	public Integer getStatus() {
		return status;
	}

	public void setStatus(Integer status) {
		this.status = status;
	}

	public Integer getIsUserDelete() {
		return isUserDelete;
	}

	public void setIsUserDelete(Integer isUserDelete) {
		this.isUserDelete = isUserDelete;
	}

	public Integer getCallForOther() {
		return callForOther;
	}

	public void setCallForOther(Integer callForOther) {
		this.callForOther = callForOther;
	}

	public Integer getCityCode() {
		return cityCode;
	}

	public void setCityCode(Integer cityCode) {
		this.cityCode = cityCode;
	}

	public Integer getAreaCode() {
		return areaCode;
	}

	public void setAreaCode(Integer areaCode) {
		this.areaCode = areaCode;
	}

	public Integer getCancelReason() {
		return cancelReason;
	}

	public void setCancelReason(Integer cancelReason) {
		this.cancelReason = cancelReason;
	}

	public String getCancelReasonMark() {
		return cancelReasonMark;
	}

	public void setCancelReasonMark(String cancelReasonMark) {
		this.cancelReasonMark = cancelReasonMark;
	}

	public Integer getLevel() {
		return level;
	}

	public void setLevel(Integer level) {
		this.level = level;
	}

	public Integer getOrgType() {
		return orgType;
	}

	public void setOrgType(Integer orgType) {
		this.orgType = orgType;
	}

	public Integer getRelationId() {
		return relationId;
	}

	public void setRelationId(Integer relationId) {
		this.relationId = relationId;
	}

	public Integer getPayType() {
		return payType;
	}

	public void setPayType(Integer payType) {
		this.payType = payType;
	}

	public Integer getDispatchCategory() {
		return dispatchCategory;
	}

	public void setDispatchCategory(Integer dispatchCategory) {
		this.dispatchCategory = dispatchCategory;
	}

	public Integer getIsAsync() {
		return isAsync;
	}

	public void setIsAsync(Integer isAsync) {
		this.isAsync = isAsync;
	}

	public Integer getOrderScore() {
		return orderScore;
	}

	public void setOrderScore(Integer orderScore) {
		this.orderScore = orderScore;
	}

	public Integer getSmsWarn() {
		return smsWarn;
	}

	public void setSmsWarn(Integer smsWarn) {
		this.smsWarn = smsWarn;
	}

	public Integer getMileage() {
		return mileage;
	}

	public void setMileage(Integer mileage) {
		this.mileage = mileage;
	}

	public String getOrderDetail() {
		return orderDetail;
	}

	public void setOrderDetail(String orderDetail) {
		this.orderDetail = orderDetail;
	}

	public String getUserComment() {
		return userComment;
	}

	public void setUserComment(String userComment) {
		this.userComment = userComment;
	}

	public String getDriverComment() {
		return driverComment;
	}

	public void setDriverComment(String driverComment) {
		this.driverComment = driverComment;
	}

	public Double getDriverScore() {
		return driverScore;
	}

	public void setDriverScore(Double driverScore) {
		this.driverScore = driverScore;
	}

	public Integer getPassengerDel() {
		return passengerDel;
	}

	public void setPassengerDel(Integer passengerDel) {
		this.passengerDel = passengerDel;
	}

	public Integer getDriverDel() {
		return driverDel;
	}

	public void setDriverDel(Integer driverDel) {
		this.driverDel = driverDel;
	}

	public Double getUserScore() {
		return userScore;
	}

	public void setUserScore(Double userScore) {
		this.userScore = userScore;
	}

	public String getPassengerPhone() {
		return passengerPhone;
	}

	public void setPassengerPhone(String passengerPhone) {
		this.passengerPhone = passengerPhone;
	}

	public String getAgencyNumber() {
		return agencyNumber;
	}

	public void setAgencyNumber(String agencyNumber) {
		this.agencyNumber = agencyNumber;
	}

	public Date getUpdateTime() {
		return updateTime;
	}

	public void setUpdateTime(Date updateTime) {
		this.updateTime = updateTime;
	}

	public Integer getPartitionId() {
		return partitionId;
	}

	public void setPartitionId(Integer partitionId) {
		this.partitionId = partitionId;
	}

	public Integer getStatement() {
		return statement;
	}

	public void setStatement(Integer statement) {
		this.statement = statement;
	}

	public Integer getCheckStatus() {
		return checkStatus;
	}

	public void setCheckStatus(Integer checkStatus) {
		this.checkStatus = checkStatus;
	}

	public Date getCreateTime() {
		return createTime;
	}

	public void setCreateTime(Date createTime) {
		this.createTime = createTime;
	}

	public String getDataSource() {
		return dataSource;
	}

	public void setDataSource(String dataSource) {
		this.dataSource = dataSource;
	}
}
