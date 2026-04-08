# Hospital Patient Management and Emergency Prioritization System

This capstone assignment requires you to design and implement a realistic hospital patient
management system that models the flow of patients through a healthcare facility. The system
should simulate key operations such as patient registration, classification of cases, queue
management, emergency prioritization, and record retrieval. The goal is to apply appropriate data
structures to improve efficiency, organization, and reliability in managing patient flow.
## System Requirements (Logical Flow)
#### 1. Patient Entry and Registration
Capture patient details (ID, name, age, condition, urgency level), create a patient record, and store
it in a tree-based structure for efficient retrieval.
#### 2. Patient Classification (Triage)
Classify patients into normal or emergency cases based on urgency level. This determines how
they will be processed.
#### 3. Queue Allocation and Management
Normal patients must be placed in a FIFO queue. Emergency patients must be prioritized using a
separate queue or priority mechanism. The system should support enqueue, dequeue, and queue
display operations.
#### 4. Patient Processing and Service
Patients are removed from the appropriate queue, attended to, and their status updated
accordingly.
#### 5. Patient Record Retrieval
Allow efficient searching of patient records using a tree structure (BST or AVL) and display full
patient details.
#### 6. Patient History Management
Maintain a linked list of patient history. Allow adding new visit records and displaying complete
history.
#### 7. Undo and Error Recovery
Use a stack to track operations and allow reversal of the most recent action such as registration or
update.
#### 8. Structured Data Management Using Arrays
Use arrays for fixed-size or indexed data such as appointment slots, department allocation, or
summary statistics.
#### 9. System Output and Interaction
Provide clear outputs including queue status, patient details, and system messages. Implement a
console interface for interaction.
### Dataset and Testing
Each group must prepare a dataset of at least 10–20 patient records including both normal and
emergency cases. Additionally, at least 5 test cases must be designed to validate system
functionality.
