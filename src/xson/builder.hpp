#pragma once

#include <stack>
#include "gsl/not_null.hpp"
#include "xson/object.hpp"

namespace xson {

class observer
{
public:
    virtual void start_object() = 0;
    virtual void end_object() = 0;
    virtual void start_array() = 0;
    virtual void end_array() = 0;
    virtual void name(xson::string_type) = 0;
    virtual void value(xson::string_type) = 0;
    virtual void value(xson::boolean_type) = 0;
    virtual void value(xson::null_type) = 0;
    virtual void value(xson::integer_type) = 0;
    virtual void value(xson::number_type) = 0;
    virtual void value(xson::timestamp_type) = 0;
};

class builder : public observer
{
public:

    xson::object get()
    {
        return std::move(m_root);
    }

private:

    void start_object()
    {
        TRACE('!');
        if(m_stack.empty())
        {
            m_root = object{object::map{}};
            m_stack.push(m_root);
        }
        else if(m_type == type::object)
        {
            m_stack.top().get().get<object::map>().emplace(m_current,object::map{});
            m_stack.push(m_stack.top().get().get<object::map>().at(m_current));
        }
        else // type::array
        {
            m_stack.top().get().get<object::array>().emplace_back(object::map{});
            m_stack.push(m_stack.top().get().get<object::array>().back());
        }
        m_type = type::object;
    }

    void end_object()
    {
        TRACE('!');
        m_stack.pop();
        if(not m_stack.empty())
        {
            if(m_stack.top().get().is_object())
                m_type = type::object;
            else if(m_stack.top().get().is_array())
                m_type = type::array;
            else
                std::terminate(); // error
        }
    }

    void start_array()
    {
        TRACE('!');
        if(m_stack.empty())
        {
            m_root = object{object::array{}};
            m_stack.push(m_root);
        }
        else if(m_type == type::object)
        {
            m_stack.top().get().get<object::map>().emplace(m_current,object::array{});
            m_stack.push(m_stack.top().get().get<object::map>().at(m_current));
        }
        else // type::array
        {
            m_stack.top().get().get<object::array>().emplace_back(object::array{});
            m_stack.push(m_stack.top().get().get<object::array>().back());
        }
        m_type = type::array;
    }

    void end_array()
    {
        TRACE('!');
        m_stack.pop();
        if(not m_stack.empty())
        {
            if(m_stack.top().get().is_object())
                m_type = type::object;
            else if(m_stack.top().get().is_array())
                m_type = type::array;
            else
                std::terminate(); // error
        }
    }

    void name(xson::string_type str)
    {
        TRACE(str);
        m_current = std::move(str);
    }

    void value(xson::string_type s)
    {
        TRACE(s);
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,s);
        else
            m_stack.top().get().get<object::array>().emplace_back(s);
    }

    void value(xson::boolean_type b)
    {
        TRACE(b);
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,b);
        else
            m_stack.top().get().get<object::array>().emplace_back(b);
    }

    void value(xson::null_type)
    {
        TRACE("null");
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,object::value{});
        else
            m_stack.top().get().get<object::array>().emplace_back(object::value{});
    }

    void value(xson::integer_type i)
    {
        TRACE(i);
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,i);
        else
            m_stack.top().get().get<object::array>().emplace_back(i);
    }

    void value(xson::number_type n)
    {
        TRACE(n);
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,n);
        else
            m_stack.top().get().get<object::array>().emplace_back(n);
    }

    void value(xson::timestamp_type dt)
    {
        TRACE('!');
        if(m_type == type::object)
            m_stack.top().get().get<object::map>().emplace(m_current,dt);
        else
            m_stack.top().get().get<object::array>().emplace_back(dt);
    }

    using stack_type = std::stack<std::reference_wrapper<xson::object>>;

    enum class type {object,array};

    type m_type = type{};

    stack_type m_stack = stack_type{};

    xson::string_type m_current = ""s;

    xson::object m_root;

}; // class builder

} // namespace xson
