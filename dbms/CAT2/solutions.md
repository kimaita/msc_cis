# QUESTIONS

## 1. Law Firm Case Management System

A medium-sized law firm maintains a spreadsheet to track client cases. The following unnormalized table represents their current data:  
`LawCases(Case_ID, Client_Name, Client_Contact, Case_Type, Lawyer_Name, Lawyer_Specialization, Hearing_Dates, Case_Status, Total_Fees, Payment_Dates, Payment_Amounts)`

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the firm’s ability to track overdue payments for each case.

1NF  
Functional Dependencies

- LawCases - Case_ID, Client_Name, Client_Contact, Case_Type, Lawyer_Name, Lawyer_Specialization,  Case_Status, Total_Fees
- Hearings - Case_ID, Hearing_Dates
- Payments - Case_ID, Payment_Dates, Payment_Amounts

2NF
Functional Dependencies

- LawCases - Case_ID, Client_Name, Client_Contact, Case_Type, Lawyer_Name, Lawyer_Specialization,  Case_Status, Total_Fees
- Hearings - Case_ID, Hearing_Dates
- Payments - Case_ID, Payment_Dates, Payment_Amounts

3NF

- LawCases - Case_ID, Case_Type, Case_Status, Total_Fees
- Clients - Client_Name, Client_Contact,
- Lawyers - Lawyer_Name, Lawyer_Specialization,
- Hearings - Case_ID, Hearing_Dates
- Payments - Case_ID, Payment_Dates, Payment_Amounts

## 2. Wholesale Distributor

A wholesale distributor records sales transactions in the following relation:  
`SalesRecord(SaleID, SaleDate, CustomerID, CustomerName, CustomerAddress, ProductList, ProductCategoryList, UnitPriceList, QuantityList, SalesRepID, SalesRepName, SalesRepRegion, RegionManager)`  

**Business Rules:** ProductList, ProductCategoryList, UnitPriceList and QuantityList are comma-separated lists (one entry per product in the sale). A SalesRep works in exactly one region. Each region has a single RegionManager. ProductCategory is an attribute of Product.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the distributor’s ability to record sales transactions correctly.

1NF

- SalesRecord(SaleID, SaleDate, CustomerID, CustomerName, CustomerAddress, SalesRepID, SalesRepName, SalesRepRegion, RegionManager)

- SaleProducts(SaleID, Product, ProductCategory, UnitPrice, Quantity)

2NF

- SalesRecord(SaleID, SaleDate, CustomerID, SalesRepID)
- SaleProducts(SaleID, ProductID, Quantity)
- Product(ProductID, Product, ProductCategory, UnitPrice)
- Customer(CustomerID, CustomerName, CustomerAddress)
- SalesReps(SalesRepID, SalesRepName, SalesRepRegion, RegionManager)

3NF

- SalesRecord(SaleID, SaleDate, CustomerID, SalesRepID)
- SaleProduct(SaleID, ProductID, Quantity)
- Product(ProductID, Product, ProductCategory, UnitPrice)
- Customer(CustomerID, CustomerName, CustomerAddress)
- SalesReps(SalesRepID, SalesRepName, SalesRepRegion)
- Region(RegionID, RegionManager)

## 3. Safari Tour Bookings

Scenario:  
`TourBooking(BookingID, BookingDate, ClientID, ClientName, ClientPhone, TourCode, TourName, TourPrice, Destination, DestinationRegion, RegionDirector, VehicleRegList, DriverList)`  

**Business Rules:** Multiple vehicles and multiple drivers may be assigned per booking. Each destination belongs to a region and each region has a single RegionDirector. A tour operates in exactly one destination.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the tour company’s ability to know all the regions which each driver has ever been allocated to.

1NF

- TourBooking(BookingID, BookingDate, ClientID, ClientName, ClientPhone, TourCode, TourName, TourPrice, Destination, DestinationRegion, RegionDirector)

- BookingDrivers(BookingID, VehicleReg, Driver)

2NF

- TourBooking(BookingID, BookingDate, ClientID, TourCode)

- BookingDrivers(BookingID, VehicleReg, Driver)

- Clients(ClientID, ClientName, ClientPhone)

- Tour(TourCode, TourName, TourPrice, Destination, DestinationRegion, RegionDirector)

3NF

- TourBooking(BookingID, BookingDate, ClientID, TourCode)

- BookingDrivers(BookingID, VehicleReg, Driver)

- Clients(ClientID, ClientName, ClientPhone)

- Tour(TourCode, TourName, TourPrice, DestinationID)

- Region(RegionID, RegionDirector)

- Destinations(DestinationID, RegionID)

## 4. Manufacturing Orders and Bill of Materials (BOM)

A manufacturer stores order and production data in the following relation:  
`ManufactureOrder(OrderID, OrderDate, CustomerID, CustomerName, ProductID, ProductName, QuantityOrdered, UnitCost, SupplierID, SupplierName, SupplierContact, BatchNo, AssemblyLineID, AssemblyLineName, LineSupervisorID, LineSupervisorName)`  

**Business Rules:** An order may include multiple product lines. A product may have multiple suppliers (preferred supplier list). An assembly line has a single supervisor.
Batch numbers are per production run of a product in an order line.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the manufacturer’s ability to record production data correctly.

1NF

- ManufactureOrder(OrderID, OrderDate, CustomerID, CustomerName, SupplierID, SupplierName, SupplierContact, BatchNo, AssemblyLineID, AssemblyLineName, LineSupervisorID, LineSupervisorName)

- OrderProducts(OrderID, ProductID, ProductName, QuantityOrdered, UnitCost)

2NF

3NF

## 5. Hospital Patient Records and Billing

A hospital stores its patient records and billing data in the following relation:  
`PatientVisit(VisitID, VisitDate, PatientID, PatientName, DOB, Address, AttendingDoctorID, AttendingDoctorName, DoctorDept, DeptHead,  DiagnosisList, MedicationList, MedicationDosageList, BillingID, TotalBillAmount, BillItemList, BillAmountList)`  

**Business Rules:**  DiagnosisList, MedicationList, MedicationDosageList, BillItemList and BillAmountList are multi-valued list fields. A doctor belongs to one department. A department has a single head.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the
functional dependencies used, indicate primary keys of resulting tables, and explain
why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the hospital’s ability to record the list
of medication without anomalies.

## 6. Real Estate Sales (Property Sales)

A real estate company records its property data in a relation based on the following schema:  
`PropertySale(SaleID, SaleDate, PropertyID, Address, PropertyType,
FeatureList, SellerID, SellerName, SellerContact, BuyerID, BuyerName,
BuyerContact, AgentList, AgentCommissionList, ClosingAttorney)`  

**Business Rules:** FeatureList and AgentList are multi-valued lists. An agent may handle multiple sales and one sale may have multiple agents (co-brokers). Each property is unique by PropertyID. Commission amounts correspond to agents on the sale.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the
functional dependencies used, indicate primary keys of resulting tables, and explain
why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the real estate company’s ability to
record the commission amount of each sales agent without anomalies.

## 7. Airline Ticketing (Multi-segment Itineraries)

Scenario and initial relation  
`Reservation(ReservationID, ReservationDate, AgentID, AgentName,
PassengerID, PassengerName, FFNumber, TotalFare, SegmentList,
SeatAssignmentList, FareClassList, TicketStatus)`  

**Business Rules:** SegmentList contains multiple flight segments for multi-leg itineraries; seat assignments and fare classes are per segment. A passenger may have a frequent flyer number.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the
functional dependencies used, indicate primary keys of resulting tables, and explain
why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the airline’s ability to record the fare
per segment without anomalies.

## 8. NGO Grant Tracking and Donor Contributions

Scenario and initial relation  
`GrantRecord(GrantID, ProjectID, ProjectName, ManagerID, ManagerName,
StartDate, EndDate, TotalBudget, DonorList, DonorAmountList,
MilestoneList, MilestoneDueDateList, BudgetLineList)`  

**Business Rules:** DonorList and DonorAmountList correspond to multiple donors and their contributions to the grant. MilestoneList is a multi-valued list of project milestones.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the
functional dependencies used, indicate primary keys of resulting tables, and explain
why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the NGO’s ability to record the list
of milestones per project without anomalies.

## 9. Insurance Claims

Scenario and initial relation  
`ClaimRecord(ClaimID, ClaimDate, PolicyNo, PolicyHolderID,
PolicyHolderName, PolicyType, IncidentDate, ClaimantID, ClaimantName,
ClaimAmount, DamagedItemList, DamagedItemValueList, AdjusterList,
AdjusterAssignmentDateList, AgentID, AgentName)`  

**Business Rules:** DamagedItemList and AdjusterList are multi-valued. A policy has a policyholder. Multiple adjusters may be assigned to a claim.

### Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the insurance provider’s ability to record claim amounts without anomalies.

## 10. E-commerce Marketplace Orders

Scenario and initial relation  
`EcomOrder(OrderID, OrderDate, CustomerID, CustomerName, CustomerEmail,
BillingAddress, ShippingAddress, OrderLineProductList,
OrderLineQuantityList, OrderLineUnitPriceList, SellerList,
WarehouseList, PromotionList, OrderTotal)`  

**Business Rules:**  OrderLineProductList and related lists are per-line; marketplace model means each product has a Seller. Warehouse assignments per order line are possible. Promotions may apply at order level or line level.

## Task

Normalize this relation step-by-step to 1NF, 2NF, and 3NF. For each step, list the functional dependencies used, indicate primary keys of resulting tables, and explain why each decomposition addresses the relevant anomalies.
Explain how this normalization process improves the e-commerce business’ ability to record the order levels without anomalies.
