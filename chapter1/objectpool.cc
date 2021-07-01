#include<iostream>
#include<memory>
#include<string>
#include<unordered_map>

using namespace std::placeholders;
class Stock{
public:    
    Stock(std::string tmp):name_(tmp) {}
    std::string GetName() { return name_; }
private:
    std::string name_;
};


class StockFactory:public std::enable_shared_from_this<StockFactory>{
public:
    StockFactory() = default;
    std::shared_ptr<Stock> Get(const std::string &tmp);
    static void DoRemove(const std::weak_ptr<StockFactory>& wptr, Stock *mystock)
    {
        std::cout << "do destory\n";
        std::shared_ptr<StockFactory> sptr(wptr.lock());
        if(sptr)
        {
            sptr->Remove(mystock);
        }
        else
        {
            std::cout << "factory has been destoried\n";
        }
        delete mystock;
    }
    void Remove(Stock* s);
private:
    std::unordered_map<std::string, std::weak_ptr<Stock>> all_stock;
};

std::shared_ptr<Stock> StockFactory::Get(const std::string &name)
{
    auto wptr = all_stock[name];
    std::shared_ptr<Stock> sptr(wptr.lock());
    if(!sptr)
    {
        sptr.reset(new Stock(name), std::bind(&StockFactory::DoRemove, static_cast<std::weak_ptr<StockFactory>>(shared_from_this()), _1));
    }
    return sptr;
}

void StockFactory::Remove(Stock* s)
{
    all_stock.erase(s->GetName());
}

void TestLongStockFactory();
void TestShortStockFactory();

int main()
{
    TestLongStockFactory();
    TestShortStockFactory();
}

void TestLongStockFactory()
{
    std::shared_ptr<StockFactory> myfactory = std::make_shared<StockFactory>();
    {
        auto google = myfactory->Get("google");
    }
}

void TestShortStockFactory()
{
    std::shared_ptr<Stock> google;
    {
        std::shared_ptr<StockFactory> myfactory = std::make_shared<StockFactory>();
        google = myfactory->Get("google");
    }
}