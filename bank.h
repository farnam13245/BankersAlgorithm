//
//  bank.h
//
//

#ifndef __BANK_H__
#define __BANK_H__

#include "customer.h"


class Bank {
public:
    Bank() = default;
    Bank(const ext_vector<int>& available) : avail(available), customers() {


    }
    // Bank(const ext_vector<int>& available, std::vector<Customer*> customers) : avail(available), customers(customers) {

    //     process = customers.size();
    //     resource = customers[0]->get_alloc().size();

    // }

    ext_vector<int> get_avail() const { return avail; }
    bool is_avail(const ext_vector<int>& req) const { return req < avail; }

    bool is_safe(int id, const ext_vector<int>& req) {
        int count = 0;
        bool execute = true;

        size_t i = 0, j = 0;
        std::vector<bool> visited(process, false);
        ext_vector<int> work = this->avail;

        while (count < process) {
            bool flag = false;
            for (i = 0; i < process; ++i) {
                execute = true;
                if (!visited[i]) {
                    if (this->customers[i]->create_req() > work) { execute = false; break; }
                    else {
                        work += this->customers[i]->get_alloc();
                        this->safe_sequence[count++] = i;
                        visited[i] = true;
                        flag = true;
                    }
                }
            }
            if (!flag) { break; }
        }
        return (count < process)
            ? false
            : true;
    }   // TODO: determine if alloc is safe

    bool req_approved(int id, const ext_vector<int>& req) {
        if (req > avail) {
            return false;
        }

        return is_safe(id, req);
    }

    void add_customer(Customer* c) {
        customers.push_back(c);
        avail -= c->allocated();

        process = customers.size();
        resource = customers[0]->get_alloc().size();
    }

    void withdraw_resources(const ext_vector<int>& req) {
        if (!is_avail(req)) {
            pthread_mutex_lock(&mutex_);
            std::cerr << "WARNING: req: " << req << " is not available for withdrawing\n";
            pthread_mutex_unlock(&mutex_);
            return;
        }
        if (is_avail(req)) { avail -= req; }
    }
    void deposit_resources(const ext_vector<int>& req) { avail += req; }


    ext_vector<Customer*> get_customers() const { return customers; }

    void show() const {
        pthread_mutex_lock(&mutex_);
        std::cout << "avail: [" << avail << "]\n";
        pthread_mutex_unlock(&mutex_);


        for (Customer* c : customers) {
            c->show();
        }
        std::cout << "\n";
    }

    friend std::ostream& operator<<(std::ostream& os, const Bank& be) {
        be.show();
        return os;
    }

private:
    ext_vector<int> avail;
    ext_vector<Customer*> customers;
    ext_vector<int> safe_sequence;

    int process;
    int resource;
};

#endif /* Bank_h */
