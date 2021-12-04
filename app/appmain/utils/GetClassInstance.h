/*
 * GetClassInstance.h
 *
 *  Created on: 2021年12月5日
 *      Author: jhting
 */

#ifndef APPMAIN_UTILS_GETCLASSINSTANCE_H_
#define APPMAIN_UTILS_GETCLASSINSTANCE_H_

/*一个单例的模板类，需要注意这个单例的对像在释放时，这个模板并不能感应到，所以请小心使用*/
template<typename T>
class GetClassInstance{
public:
    static T* getInstance()
    {
        if(!m_instace)
        {
            m_instace = new T();
        }

        return m_instace;
    }
private:
    static T* m_instace;
};

template<typename T> T* GetClassInstance<T>::m_instace = NULL;



#endif /* APPMAIN_UTILS_GETCLASSINSTANCE_H_ */
