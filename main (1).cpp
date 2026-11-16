#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>

using namespace std;

const int MAX_PRODUCTS = 200;
const int MAX_CUSTOMERS = 200;
const int MAX_ORDERS = 500;
const int MAX_CART_ITEMS = 50;

class User {
protected:
    string username;
    string password;
public:
    User() {
        username = "";
        password = "";
    }
    virtual string getRole() const = 0;
    void setLogin(const string &u, const string &p) { username = u; password = p; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    bool checkPassword(const string &p) const { return password == p; }
};

class Admin : public User {
public:
    Admin() { setLogin("admin", "admin123"); }
    string getRole() const { return "Admin"; }
};

class Product {
public:
    int id;
    string name;
    string category;
    double price;
    int stock;
    bool active;

    Product() {
        id = 0;
        name = "";
        category = "";
        price = 0;
        stock = 0;
        active = true;
    }
};

class Customer : public User {
public:
    int id;
    string name;
    string email;
    string phone;

    Customer() {
        id = 0;
        name = "";
        email = "";
        phone = "";
    }

    string getRole() const { return "Customer"; }
};

class CartItem {
public:
    int productId;
    int quantity;

    CartItem() {
        productId = 0;
        quantity = 0;
    }
};

class Order {
public:
    int id;
    int customerId;
    int priority;
    int productId;
    int quantity;
    string customerName;
    string productName;
    string status;
    double total;

    Order() {
        id = 0;
        customerId = 0;
        priority = 0;
        productId = 0;
        quantity = 0;
        customerName = "";
        productName = "";
        status = "";
        total = 0;
    }
};

class SmartCart {
private:
    Product products[MAX_PRODUCTS];
    Customer customers[MAX_CUSTOMERS];
    CartItem cart[MAX_CART_ITEMS];
    Order orderHistory[MAX_ORDERS];

    // Separate queues are used for normal and priority orders.
    Order normalQueue[MAX_ORDERS];
    Order priorityQueue[MAX_ORDERS];

    int productCount, customerCount, cartCount, orderCount;
    int nextProductId, nextCustomerId, nextOrderId;
    int front, rear, normalCount, priorityCount;
    Admin admin;
    int currentCustomerId;

    string getLine(string message) {
        string value;
        cout << message;
        getline(cin, value);
        return value;
    }

    int getInteger(string message) {
        string value;
        int number;

        while (true) {
            value = getLine(message);
            stringstream input(value);
            char extra;

            if (input >> number && !(input >> extra)) {
                return number;
            }
            cout << "Please enter a valid integer.\n";
        }
    }

    double getAmount(string message) {
        string value;
        double amount;

        while (true) {
            value = getLine(message);
            stringstream input(value);
            char extra;

            if (input >> amount && !(input >> extra) && amount > 0) {
                return amount;
            }
            cout << "Please enter a valid amount greater than zero.\n";
        }
    }

    int findProduct(int id) {
        for (int i = 0; i < productCount; i++) {
            if (products[i].id == id && products[i].active) {
                return i;
            }
        }
        return -1;
    }

    int findCustomer(int id) {
        for (int i = 0; i < customerCount; i++) {
            if (customers[i].id == id) {
                return i;
            }
        }
        return -1;
    }

    void addNormalOrder(const Order &order) {
        if (normalCount == MAX_ORDERS) {
            return;
        }

        // Circular queue: after the last array position, rear returns to zero.
        normalQueue[rear] = order;
        rear = (rear + 1) % MAX_ORDERS;
        normalCount++;
    }

    void addPriorityOrder(const Order &order) {
        if (priorityCount == MAX_ORDERS) {
            return;
        }

        // Keep the highest priority at index zero. Equal priorities stay in arrival order.
        int i = priorityCount;
        while (i > 0 && priorityQueue[i - 1].priority < order.priority) {
            priorityQueue[i] = priorityQueue[i - 1];
            i--;
        }
        priorityQueue[i] = order;
        priorityCount++;
    }

    void rebuildQueues() {
        front = 0;
        rear = 0;
        normalCount = 0;
        priorityCount = 0;

        // Waiting orders are put back into their queues when the program starts.
        for (int i = 0; i < orderCount; i++) {
            if (orderHistory[i].status == "Waiting") {
                if (orderHistory[i].priority == 0) {
                    addNormalOrder(orderHistory[i]);
                } else {
                    addPriorityOrder(orderHistory[i]);
                }
            }
        }
    }

    void addSampleProducts() {
        // Sample inventory is loaded automatically when products.txt is empty.
        addProduct("HP 15s Laptop", "Laptops", 45999, 8);
        addProduct("Dell Inspiron 14", "Laptops", 52999, 6);
        addProduct("Lenovo IdeaPad Slim 3", "Laptops", 48999, 7);
        addProduct("ASUS VivoBook 15", "Laptops", 56999, 5);
        addProduct("Acer Aspire Lite", "Laptops", 39999, 9);
        addProduct("Apple MacBook Air M2", "Laptops", 89999, 3);
        addProduct("Logitech Wireless Mouse", "Accessories", 799, 25);
        addProduct("HP Wireless Keyboard", "Accessories", 999, 18);
        addProduct("Laptop Backpack", "Accessories", 1199, 20);
        addProduct("USB-C Hub 6-in-1", "Accessories", 1499, 12);
        addProduct("Portronics Laptop Stand", "Accessories", 899, 15);
        addProduct("Samsung Galaxy S24 FE", "Phones", 54999, 6);
        addProduct("Apple iPhone 15", "Phones", 59999, 4);
        addProduct("OnePlus Nord CE 4", "Phones", 24999, 10);
        addProduct("Redmi Note 13 Pro", "Phones", 23999, 12);
        addProduct("Motorola Edge 50 Fusion", "Phones", 22999, 8);
        addProduct("Realme Narzo 70", "Phones", 15999, 11);
        addProduct("Samsung Galaxy A35", "Phones", 28999, 7);
        addProduct("boAt Airdopes 141", "Earbuds", 1299, 30);
        addProduct("Noise Buds VS104", "Earbuds", 1499, 22);
        addProduct("OnePlus Nord Buds 2", "Earbuds", 2299, 16);
        addProduct("Realme Buds T300", "Earbuds", 1999, 18);
        addProduct("JBL Wave Buds", "Earbuds", 3499, 10);
        addProduct("Sony WF-C500 Earbuds", "Earbuds", 4999, 6);
        addProduct("JBL Go 3 Speaker", "Speakers", 2999, 9);
        addProduct("boAt Stone 350 Speaker", "Speakers", 1799, 14);
        addProduct("Zebronics Zeb-County", "Speakers", 699, 20);
        addProduct("Sony SRS-XB13 Speaker", "Speakers", 3999, 7);
        addProduct("Portronics SoundDrum", "Speakers", 2499, 11);
        addProduct("Samsung 24-inch Monitor", "Monitors", 8999, 8);
        addProduct("LG 27-inch IPS Monitor", "Monitors", 14999, 5);
        addProduct("Acer 22-inch Monitor", "Monitors", 7499, 9);
        addProduct("BenQ 24-inch Monitor", "Monitors", 11999, 6);
        addProduct("Dell 27-inch FHD Monitor", "Monitors", 13999, 4);
        addProduct("Men Cotton T-Shirt", "Clothing", 499, 40);
        addProduct("Women Printed T-Shirt", "Clothing", 599, 32);
        addProduct("Men Casual Shirt", "Clothing", 999, 24);
        addProduct("Women Kurti", "Clothing", 899, 28);
        addProduct("Men Blue Jeans", "Clothing", 1299, 20);
        addProduct("Women Denim Jeans", "Clothing", 1499, 18);
        addProduct("Unisex Hoodie", "Clothing", 1199, 16);
        addProduct("Sports Track Pants", "Clothing", 799, 22);
        addProduct("Running Shoes", "Footwear", 1999, 14);
        addProduct("Casual Sneakers", "Footwear", 1799, 17);
        addProduct("Women's Walking Shoes", "Footwear", 1599, 13);
        addProduct("Smart Watch", "Wearables", 2499, 15);
        addProduct("Fitness Band", "Wearables", 1499, 19);
        addProduct("USB-C Fast Charger", "Electronics", 799, 35);
        addProduct("10000mAh Power Bank", "Electronics", 1299, 21);
        addProduct("HDMI Cable 2m", "Electronics", 299, 40);
        addProduct("Webcam 1080p", "Electronics", 1899, 12);
    }

    void addSampleCustomers() {
        // Fictional demo accounts. These plain-text passwords are for project testing only.
        Customer c;

        c.id = nextCustomerId++; c.name = "Rahul Sharma"; c.email = "rahul.demo@example.com"; c.phone = "9000000001"; c.setLogin("rahul", "rahul123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Priya Verma"; c.email = "priya.demo@example.com"; c.phone = "9000000002"; c.setLogin("priya", "priya123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Aman Gupta"; c.email = "aman.demo@example.com"; c.phone = "9000000003"; c.setLogin("aman", "aman123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Sneha Kapoor"; c.email = "sneha.demo@example.com"; c.phone = "9000000004"; c.setLogin("sneha", "sneha123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Arjun Mehta"; c.email = "arjun.demo@example.com"; c.phone = "9000000005"; c.setLogin("arjun", "arjun123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Neha Singh"; c.email = "neha.demo@example.com"; c.phone = "9000000006"; c.setLogin("neha", "neha123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Karan Patel"; c.email = "karan.demo@example.com"; c.phone = "9000000007"; c.setLogin("karan", "karan123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Ananya Joshi"; c.email = "ananya.demo@example.com"; c.phone = "9000000008"; c.setLogin("ananya", "ananya123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Rohan Malhotra"; c.email = "rohan.demo@example.com"; c.phone = "9000000009"; c.setLogin("rohan", "rohan123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Isha Agarwal"; c.email = "isha.demo@example.com"; c.phone = "9000000010"; c.setLogin("isha", "isha123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Vivek Kumar"; c.email = "vivek.demo@example.com"; c.phone = "9000000011"; c.setLogin("vivek", "vivek123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Meera Nair"; c.email = "meera.demo@example.com"; c.phone = "9000000012"; c.setLogin("meera", "meera123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Aditya Rao"; c.email = "aditya.demo@example.com"; c.phone = "9000000013"; c.setLogin("aditya", "aditya123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Pooja Mishra"; c.email = "pooja.demo@example.com"; c.phone = "9000000014"; c.setLogin("pooja", "pooja123"); customers[customerCount++] = c;
        c = Customer(); c.id = nextCustomerId++; c.name = "Dev Shah"; c.email = "dev.demo@example.com"; c.phone = "9000000015"; c.setLogin("dev", "dev123"); customers[customerCount++] = c;

        saveCustomers();
    }

    void loadData() {
        string line;

        ifstream productFile("data/products.txt");
        while (getline(productFile, line) && productCount < MAX_PRODUCTS) {
            stringstream row(line);
            string id, name, category, price, stock;
            getline(row, id, '|');
            getline(row, name, '|');
            getline(row, category, '|');
            getline(row, price, '|');
            getline(row, stock);

            if (id.empty()) continue;

            int productId = atoi(id.c_str());
            products[productCount].id = productId;
            products[productCount].name = name;
            products[productCount].category = category;
            products[productCount].price = atof(price.c_str());
            products[productCount].stock = atoi(stock.c_str());
            products[productCount].active = true;
            productCount++;

            if (productId >= nextProductId) nextProductId = productId + 1;
        }
        productFile.close();

        ifstream customerFile("data/customers.txt");
        while (getline(customerFile, line) && customerCount < MAX_CUSTOMERS) {
            stringstream row(line);
            string id, name, email, phone, username, password;
            getline(row, id, '|');
            getline(row, name, '|');
            getline(row, email, '|');
            getline(row, phone, '|');
            getline(row, username, '|');
            getline(row, password);

            if (id.empty()) continue;

            int customerId = atoi(id.c_str());
            customers[customerCount].id = customerId;
            customers[customerCount].name = name;
            customers[customerCount].email = email;
            customers[customerCount].phone = phone;
            customers[customerCount].setLogin(username, password);
            customerCount++;

            if (customerId >= nextCustomerId) nextCustomerId = customerId + 1;
        }
        customerFile.close();

        ifstream orderFile("data/orders.txt");
        while (getline(orderFile, line) && orderCount < MAX_ORDERS) {
            stringstream row(line);
            string id, customerId, priority, productId, quantity;
            string customerName, productName, total, status;

            getline(row, id, '|');
            getline(row, customerId, '|');
            getline(row, priority, '|');
            getline(row, productId, '|');
            getline(row, quantity, '|');
            getline(row, customerName, '|');
            getline(row, productName, '|');
            getline(row, total, '|');
            getline(row, status);

            if (id.empty()) continue;

            int orderId = atoi(id.c_str());
            orderHistory[orderCount].id = orderId;
            orderHistory[orderCount].customerId = atoi(customerId.c_str());
            orderHistory[orderCount].priority = atoi(priority.c_str());
            orderHistory[orderCount].productId = atoi(productId.c_str());
            orderHistory[orderCount].quantity = atoi(quantity.c_str());
            orderHistory[orderCount].customerName = customerName;
            orderHistory[orderCount].productName = productName;
            orderHistory[orderCount].total = atof(total.c_str());
            orderHistory[orderCount].status = status;
            orderCount++;

            if (orderId >= nextOrderId) nextOrderId = orderId + 1;
        }
        orderFile.close();
    }

public:
    SmartCart() {
        productCount = 0;
        customerCount = 0;
        cartCount = 0;
        orderCount = 0;

        nextProductId = 1001;
        nextCustomerId = 1;
        nextOrderId = 5001;

        front = 0;
        rear = 0;
        normalCount = 0;
        priorityCount = 0;
        currentCustomerId = -1;

        loadData();

        if (productCount == 0) {
            addSampleProducts();
        }
        if (customerCount == 0) {
            addSampleCustomers();
        }
        rebuildQueues();
    }

    void saveProducts() {
        ofstream file("data/products.txt");
        for (int i = 0; i < productCount; i++) {
            if (products[i].active) {
                file << products[i].id << '|'
                     << products[i].name << '|'
                     << products[i].category << '|'
                     << products[i].price << '|'
                     << products[i].stock << '\n';
            }
        }
    }

    void saveCustomers() {
        ofstream file("data/customers.txt");
        for (int i = 0; i < customerCount; i++) {
            file << customers[i].id << '|'
                 << customers[i].name << '|'
                 << customers[i].email << '|'
                 << customers[i].phone << '|' << customers[i].getUsername() << '|' << customers[i].getPassword() << '\n';
        }
    }

    void saveOrders() {
        ofstream file("data/orders.txt");
        for (int i = 0; i < orderCount; i++) {
            file << orderHistory[i].id << '|'
                 << orderHistory[i].customerId << '|'
                 << orderHistory[i].priority << '|'
                 << orderHistory[i].productId << '|'
                 << orderHistory[i].quantity << '|'
                 << orderHistory[i].customerName << '|'
                 << orderHistory[i].productName << '|'
                 << orderHistory[i].total << '|'
                 << orderHistory[i].status << '\n';
        }
    }

    void saveAll() {
        saveProducts();
        saveCustomers();
        saveOrders();
    }

    void showProducts() {
        cout << "\n---------------- PRODUCT LIST ----------------\n";
        cout << left << setw(8) << "ID" << setw(26) << "Name"
             << setw(18) << "Category" << right << setw(10) << "Price"
             << setw(8) << "Stock" << '\n';

        for (int i = 0; i < productCount; i++) {
            if (products[i].active) {
                cout << left << setw(8) << products[i].id
                     << setw(26) << products[i].name.substr(0, 24)
                     << setw(18) << products[i].category.substr(0, 16)
                     << right << setw(10) << fixed << setprecision(2)
                     << products[i].price << setw(8) << products[i].stock << '\n';
            }
        }
    }

    void addProduct(string name, string category, double price, int stock) {
        if (productCount >= MAX_PRODUCTS || name.empty() || category.empty()
            || price <= 0 || stock < 0) {
            cout << "Product details are not valid or the list is full.\n";
            return;
        }

        Product p;
        p.id = nextProductId++;
        p.name = name;
        p.category = category;
        p.price = price;
        p.stock = stock;
        p.active = true;

        products[productCount++] = p;
        saveProducts();
        cout << "Product added. Product ID: " << p.id << '\n';
    }

    void addProductMenu() {
        string name = getLine("Product name: ");
        string category = getLine("Category: ");
        double price = getAmount("Price: Rs. ");
        int stock = getInteger("Available stock: ");

        if (stock < 0) {
            cout << "Stock cannot be negative.\n";
            return;
        }
        addProduct(name, category, price, stock);
    }

    void updateProduct() {
        int id = getInteger("Enter product ID: ");
        int index = findProduct(id);

        if (index == -1) {
            cout << "Product was not found.\n";
            return;
        }

        string name = getLine("New name: ");
        string category = getLine("New category: ");
        double price = getAmount("New price: Rs. ");
        int stock = getInteger("New stock: ");

        if (name.empty() || category.empty() || stock < 0) {
            cout << "Invalid details. Product was not changed.\n";
            return;
        }

        products[index].name = name;
        products[index].category = category;
        products[index].price = price;
        products[index].stock = stock;
        saveProducts();
        cout << "Product details updated.\n";
    }

    void deleteProduct() {
        int id = getInteger("Enter product ID to remove: ");
        int index = findProduct(id);

        if (index == -1) {
            cout << "Product was not found.\n";
            return;
        }

        // We mark it inactive instead of shifting the remaining records.
        products[index].active = false;
        saveProducts();
        cout << "Product removed from the catalogue.\n";
    }

    int registerCustomer() {
        if (customerCount >= MAX_CUSTOMERS) {
            cout << "Customer list is full.\n";
            return -1;
        }

        string name = getLine("Your name: ");
        string email = getLine("Email: ");
        string phone = getLine("Phone number: ");
        string username = getLine("Choose username: ");
        string password = getLine("Choose password: ");

        if (name.empty() || email.find('@') == string::npos || phone.empty() || username.empty() || password.empty()) {
            cout << "Please enter valid customer details.\n";
            return -1;
        }
        if (findCustomerByUsername(username) != -1) {
            cout << "That username is already taken. Please choose another one.\n";
            return -1;
        }

        Customer c;
        c.id = nextCustomerId++;
        c.name = name;
        c.email = email;
        c.phone = phone;
        c.setLogin(username, password);

        customers[customerCount++] = c;
        saveCustomers();
        cout << "Customer account created. Your ID is " << c.id << '\n';
        return c.id;
    }

    void addToCart() {
        if (cartCount == MAX_CART_ITEMS) {
            cout << "Cart is full.\n";
            return;
        }

        showProducts();
        int id = getInteger("Enter product ID: ");
        int quantity = getInteger("Enter quantity: ");
        int index = findProduct(id);

        if (index == -1 || quantity <= 0) {
            cout << "Product ID or quantity is not valid.\n";
            return;
        }

        int cartIndex = -1;
        for (int i = 0; i < cartCount; i++) {
            if (cart[i].productId == id) cartIndex = i;
        }

        int oldQuantity = (cartIndex == -1) ? 0 : cart[cartIndex].quantity;
        if (products[index].stock < oldQuantity + quantity) {
            cout << "Not enough stock available.\n";
            return;
        }

        if (cartIndex == -1) {
            cart[cartCount].productId = id;
            cart[cartCount].quantity = quantity;
            cartCount++;
        } else {
            cart[cartIndex].quantity += quantity;
        }

        cout << "Item added to cart.\n";
    }

    void showCart() {
        cout << "\n---------------- YOUR CART ----------------\n";
        if (cartCount == 0) {
            cout << "Cart is empty.\n";
            return;
        }

        double grandTotal = 0;
        for (int i = 0; i < cartCount; i++) {
            int index = findProduct(cart[i].productId);
            if (index != -1) {
                double amount = products[index].price * cart[i].quantity;
                cout << products[index].name << " | Quantity: " << cart[i].quantity
                     << " | Rs. " << fixed << setprecision(2) << amount << '\n';
                grandTotal += amount;
            }
        }
        cout << "Total amount: Rs. " << grandTotal << '\n';
    }

    void removeFromCart() {
        if (cartCount == 0) {
            cout << "Cart is empty.\n";
            return;
        }

        int id = getInteger("Enter product ID to remove: ");
        int index = -1;
        for (int i = 0; i < cartCount; i++) {
            if (cart[i].productId == id) index = i;
        }

        if (index == -1) {
            cout << "That item is not in the cart.\n";
            return;
        }

        for (int i = index; i < cartCount - 1; i++) {
            cart[i] = cart[i + 1];
        }
        cartCount--;
        cout << "Item removed from cart.\n";
    }

    void checkout() {
        if (cartCount == 0) {
            cout << "Add something to the cart before checkout.\n";
            return;
        }

        if (orderCount + cartCount > MAX_ORDERS) {
            cout << "Order history is full. Cannot place this order.\n";
            return;
        }

        // Check every item first so that stock is not partly reduced if one item is unavailable.
        for (int i = 0; i < cartCount; i++) {
            int index = findProduct(cart[i].productId);
            if (index == -1 || products[index].stock < cart[i].quantity) {
                cout << "Checkout stopped. Please check the stock again.\n";
                return;
            }
        }

        int customerId = currentCustomerId;
        if (customerId == -1) { cout << "Please log in first.\n"; return; }

        int priority = getInteger("Order type (0-Normal, 1-High, 2-Urgent): ");
        if (priority < 0 || priority > 2) {
            cout << "Invalid order type. Please try checkout again.\n";
            return;
        }

        int customerIndex = findCustomer(customerId);
        double bill = 0;

        for (int i = 0; i < cartCount; i++) {
            int productIndex = findProduct(cart[i].productId);
            Product &p = products[productIndex];

            Order order;
            order.id = nextOrderId++;
            order.customerId = customerId;
            order.priority = priority;
            order.productId = p.id;
            order.quantity = cart[i].quantity;
            order.customerName = customers[customerIndex].name;
            order.productName = p.name;
            order.total = p.price * cart[i].quantity;
            order.status = "Waiting";

            orderHistory[orderCount++] = order;
            p.stock -= cart[i].quantity;

            if (priority == 0) addNormalOrder(order);
            else addPriorityOrder(order);

            bill += order.total;
            cout << "Order created: #" << order.id << " - " << order.productName << '\n';
        }

        cartCount = 0;
        saveAll();
        cout << "Checkout completed. Bill amount: Rs. "
             << fixed << setprecision(2) << bill << '\n';
    }

    void processOrder(bool processPriority) {
        Order currentOrder;

        if (processPriority) {
            if (priorityCount == 0) {
                cout << "There are no priority orders waiting.\n";
                return;
            }

            currentOrder = priorityQueue[0];
            for (int i = 0; i < priorityCount - 1; i++) {
                priorityQueue[i] = priorityQueue[i + 1];
            }
            priorityCount--;
        } else {
            if (normalCount == 0) {
                cout << "There are no normal orders waiting.\n";
                return;
            }

            currentOrder = normalQueue[front];
            front = (front + 1) % MAX_ORDERS;
            normalCount--;
        }

        for (int i = 0; i < orderCount; i++) {
            if (orderHistory[i].id == currentOrder.id) {
                orderHistory[i].status = "Processed";
                break;
            }
        }

        saveOrders();
        cout << "Order #" << currentOrder.id << " processed for "
             << currentOrder.customerName << " (" << currentOrder.productName << ").\n";
    }

    void showOrderHistory() {
        cout << "\n---------------- ORDER HISTORY ----------------\n";
        if (orderCount == 0) {
            cout << "No orders have been placed yet.\n";
            return;
        }

        for (int i = 0; i < orderCount; i++) {
            string type = "Normal";
            if (orderHistory[i].priority == 1) type = "High";
            if (orderHistory[i].priority == 2) type = "Urgent";

            cout << "Order #" << orderHistory[i].id
                 << " | " << orderHistory[i].customerName
                 << " | " << orderHistory[i].productName
                 << " | Qty: " << orderHistory[i].quantity
                 << " | Rs. " << fixed << setprecision(2) << orderHistory[i].total
                 << " | " << type << " | " << orderHistory[i].status << '\n';
        }
    }

    void showQueueStatus() {
        cout << "Normal orders waiting: " << normalCount << " (FIFO queue)\n";
        cout << "Priority orders waiting: " << priorityCount
             << " (Urgent orders are processed first)\n";
    }

    int findCustomerByUsername(const string &username) {
        for (int i = 0; i < customerCount; i++) {
            if (customers[i].getUsername() == username) return i;
        }
        return -1;
    }

    bool adminLogin() {
        string username = getLine("Admin username: ");
        string password = getLine("Admin password: ");
        if (username == admin.getUsername() && admin.checkPassword(password)) return true;
        cout << "Incorrect admin username or password.\n";
        return false;
    }

    bool customerLogin() {
        string username = getLine("Username: ");
        string password = getLine("Password: ");
        int index = findCustomerByUsername(username);
        if (index == -1 || !customers[index].checkPassword(password)) {
            cout << "Login details did not match.\n";
            return false;
        }
        currentCustomerId = customers[index].id;
        cout << "Welcome, " << customers[index].name << "!\n";
        return true;
    }

    void showMyOrders() {
        bool found = false;
        cout << "\n--------------- MY ORDERS ---------------\n";
        for (int i = 0; i < orderCount; i++) {
            if (orderHistory[i].customerId == currentCustomerId) {
                found = true;
                cout << "Order #" << orderHistory[i].id << " | "
                     << orderHistory[i].productName << " | Qty: "
                     << orderHistory[i].quantity << " | Rs. "
                     << fixed << setprecision(2) << orderHistory[i].total
                     << " | " << orderHistory[i].status << '\n';
            }
        }
        if (!found) cout << "You have not placed any orders yet.\n";
    }

    void adminMenu() {
        int choice;
        do {
            cout << "\n========== ADMIN PANEL ==========\n"
                 << "1. View products\n2. Add product\n3. Update product\n"
                 << "4. Remove product\n5. View all orders\n"
                 << "6. Process normal order (FIFO)\n7. Process priority order\n"
                 << "8. View waiting queue count\n0. Logout\n";
            choice = getInteger("Enter choice: ");
            switch (choice) {
                case 1: showProducts(); break;
                case 2: addProductMenu(); break;
                case 3: updateProduct(); break;
                case 4: deleteProduct(); break;
                case 5: showOrderHistory(); break;
                case 6: processOrder(false); break;
                case 7: processOrder(true); break;
                case 8: showQueueStatus(); break;
                case 0: cout << "Admin logged out.\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    void customerMenu() {
        int choice;
        cartCount = 0;
        do {
            cout << "\n========== CUSTOMER MENU ==========\n"
                 << "1. Browse products\n2. Add item to cart\n"
                 << "3. View cart\n4. Remove item from cart\n"
                 << "5. Checkout\n6. View my orders\n0. Logout\n";
            choice = getInteger("Enter choice: ");
            switch (choice) {
                case 1: showProducts(); break;
                case 2: addToCart(); break;
                case 3: showCart(); break;
                case 4: removeFromCart(); break;
                case 5: checkout(); break;
                case 6: showMyOrders(); break;
                case 0: currentCustomerId = -1; cartCount = 0; cout << "Customer logged out.\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 0);
    }

    void start() {
        int choice;
        do {
            cout << "\n========== SMARTCART E-COMMERCE SYSTEM ==========\n"
                 << "1. Admin Login\n2. Customer Login\n"
                 << "3. New Customer Registration\n0. Exit\n";
            choice = getInteger("Enter choice: ");
            if (choice == 1) {
                if (adminLogin()) adminMenu();
            } else if (choice == 2) {
                if (customerLogin()) customerMenu();
            } else if (choice == 3) {
                int id = registerCustomer();
                if (id != -1) {
                    currentCustomerId = id;
                    customerMenu();
                }
            } else if (choice == 0) {
                saveAll();
                cout << "Data saved. Thank you for using SmartCart!\n";
            } else {
                cout << "Please select a valid option.\n";
            }
        } while (choice != 0);
    }


};

int main() {
    SmartCart store;
    store.start();
    return 0;
}
