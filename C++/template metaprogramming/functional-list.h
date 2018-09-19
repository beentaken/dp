/******************************************************************************/
/*!
\file   functional-list.h
\author Chan Yu Hong
\par    email: yuhong.chan\@digipen.edu
\par    DigiPen login: yuhong.chan
\par    Course: CS225
\par    Assignment: 5
\date   27-07-2016
\brief
    This is the header that provide facilities for a compile time list
    of non-type template arguments.
    
    Hours spent on this assignment: 24 hrs
    Specific portions that gave you the most trouble: 
      Partition, 
      Print(20000 elements). 
*/
/******************************************************************************/
#ifndef FUNCTIONAL_LIST_H
#define FUNCTIONAL_LIST_H

#include <utility>            //std::conditional_t
#include <type_traits>        //std::is_same, std::remove_cv
#include <initializer_list>   //std::initializer_list<int>
/******************************************************************************/
/*!
  \var dummyInt
  \brief
    This variable is used to expand the parameter pack when printing correctly
    It is limited to file scope by using static.
*/
/******************************************************************************/
static const int dummyInt = 0;
/******************************************************************************/
/*!
  \struct Facility
  \brief
    The container class that contains all of the facilities needed
    to manipulate list elements at compile time.
  \tparam T
    The type of the elements that the Facility will work on
  \tparam Comp
    The comparision object that is used for functions like Sort and Min
    in facility
*/
/******************************************************************************/
template <typename T = int, typename Comp = std::less<int>>
struct Facility
{ 
/******************************************************************************/
/*!
  \struct List
  \brief 
    Creates a compile time list of non-type values of type T which 
    are stored in the template parameters.
    List also contains a function that will print out the values in the
    list.
*/
/******************************************************************************/
  template <T... elems>
  struct List
  {
  private:
/******************************************************************************/
/*!
  \brief
    This function does nothing and is just a wrapper to expand
    the initializer list
  \param unnamed
    Uses a unnamed parameter to suppress compiler warnings. 
*/
/******************************************************************************/
    static void UnusedFunc(const std::initializer_list<int>&) {}
/******************************************************************************/
/*!
  \struct DummyList
  \brief  
    Used as a dummy container to forward parameter packs
*/
/******************************************************************************/
    
    template <T... values>
    struct DummyList;
/******************************************************************************/
/*!
  \struct Print_Impl
  \brief  
    This is the base class template of Print_Impl
*/
/******************************************************************************/
    template <typename L, bool enable = std::is_same<
                                        typename std::remove_cv<T>::type, 
                                        char>::value>
    struct Print_Impl;
/******************************************************************************/
/*!
  \struct Print_Impl
  \brief
    This is a specialization for Print_Impl for the general case
    when the type is not char (this will cause the last boolean
    to evaluate to false)
*/
/******************************************************************************/
    template <T Head, T... Rest>
    struct Print_Impl<DummyList<Head, Rest...>, false>
    {
/******************************************************************************/
/*!
  \brief
    A static function that prints out the elements using
    the format given in the correct output.
    This print function prints integers and prints different
    from the specialization for chars.
*/
/******************************************************************************/
      static void print()
      {
        std::cout << "\"" << Head;
        UnusedFunc({ (std::cout <<  " " << Rest, dummyInt)... });
        std::cout << "\"" << std::endl;
      }
    }; 
/******************************************************************************/
/*!
  \struct Print_Impl
  \brief  
    This is a specialization for Print_Impl when the template
    parameter of Facility is char
*/
/******************************************************************************/
    template <T Head, T... Rest>
    struct Print_Impl<DummyList<Head, Rest...>, true>
    {
/******************************************************************************/
/*!
  \brief
    A static function that prints out the elements using
    the format given in the correct output.
    This print function prints chars.
*/
/******************************************************************************/
      static void print()
      {
        std::cout << "\"" << Head;
        UnusedFunc({ (std::cout << Rest, dummyInt)... });
        std::cout << "\"" << std::endl;
      }
    }; 
/******************************************************************************/
/*!
  \struct Print_Impl
  \brief
    This is a specialization of Print_Impl for the case when list is empty
*/
/******************************************************************************/
    //
    template <bool B>
    struct Print_Impl<DummyList<>, B>
    {
/******************************************************************************/
/*!
  \brief
    Just prints out: "Empty List"
*/
/******************************************************************************/
      static void print()
      {
        std::cout << "\"Empty List\"" << std::endl;
      }
    };
  public:  
/******************************************************************************/
/*!
  \brief
     The print function of Facility<T>::List.
     Instantiates the Print_Impl structs that will print differently
     based on the template parameter and type of T.
     As it only instantiates one Print_Impl template, instead of
     recursively instantiating multiple templates based on the list size,
     it is possible to print 20000 elements.
*/
/******************************************************************************/
    static void print()
    {
      Print_Impl<DummyList<elems...>>::print();
    }
  };//end Facility<T>::List

/******************************************************************************/
/*!
  \struct DotProduct
  \brief 
     Base class template of DotProduct.
     DotProduct is a metafunction that returns the result of a dotproduct
     on each of the elements in two lists, in order.
     This base class template declaration allows us to 
     capture 2 parameter packs in its specializations.
*/
/******************************************************************************/
  template <typename List1, typename List2>
  struct DotProduct;
/******************************************************************************/
/*!
  \struct DotProduct
  \brief
    Specialization of DotProduct that allows us to capture the Head of each
    parameter pack to DotProduct them together.
    This is the general case specialization for DotProduct
*/
/******************************************************************************/
  template <T Head1, T Head2, T... elems1, T... elems2>
  struct DotProduct< List<Head1, elems1...>, List<Head2, elems2...> >
  {
    //Multiply the two heads and add them to a recursive call of DotProduct
    constexpr static T result = 
      Head1 * Head2 + DotProduct<List<elems1...>, List<elems2...>>::result;
  };
/******************************************************************************/
/*!
  \struct DotProduct
  \brief
    Specialization of DotProduct for the base case when there is only
    one element left in each list.
*/
/******************************************************************************/
  template <T Head1, T Head2>
  struct DotProduct< List<Head1>, List<Head2>>
  {
    //just return Head1 * Head2 and end the recursion
    constexpr static T result = Head1 * Head2;
  };
/******************************************************************************/
/*!
  \struct DotProduct
  \brief
    Specialization of DotProduct for the case when both of the lists
    are empty.
    Also checks if both of the lists are of the same size and forces
    a compile error if they are not.
*/
/******************************************************************************/
  template <T... Head1, T... Head2>
  struct DotProduct< List<Head1...>, List<Head2...>>
  {
    //return result as the default constructor of T (most cases : 0)
    constexpr static T result = T();
    /* to check if both lists are the same size. This will cause a compile
    failure if the 2 lists are of unequal size. */
    using CheckIfSameSize = 
      typename std::enable_if<sizeof...(Head1) == sizeof...(Head2)>::type;
  };
/******************************************************************************/
/*!
  \struct Concat
  \brief 
    Concat is a metafunction that concatenates 2 lists.
    This is the base class template for concat that allows us to capture
    2 parameter packs.
*/
/******************************************************************************/
  template <typename List1, typename List2>
  struct Concat;
/******************************************************************************/
/*!
  \struct Concat
  \brief 
    This is the specialization for Concat with 2 parameter packs that
    allows us to combine the parameter packs together into one
    parameter pack
*/
/******************************************************************************/
  template <T... elems1, T... elems2>
  struct Concat< List<elems1...>, List<elems2...> >
  {
    //combine both parameter packs
    using result = List<elems1..., elems2...>;
  };
/******************************************************************************/
/*!
  \struct RemoveFirst
  \brief
    RemoveFirst is a metafunction that takes a list and an item. 
    It removes the first occurence of the item on the list. If the item is 
    not found, then the list remains as what it ought to be.
    Base class template for the RemoveFirst struct
  \tparam Num
    The number to look for in the list
  \tparam L
    The the type that will contain the list.
*/
/******************************************************************************/
  template <T Num, typename L>
  struct RemoveFirst;
/******************************************************************************/
/*!
  \struct RemoveFirst
  \brief
    Specialization of RemoveFirst for the general case where Num and Head
    are different values. Head is taken out of the list and Concat is used
    to recursively concatenate it with another call to RemoveFirst.
*/
/******************************************************************************/
  template <T Num, T Head, T... elems>
  struct RemoveFirst<Num, List<Head, elems...> >
  {
    using result = typename Concat<List<Head>, 
                    typename RemoveFirst<Num, List<elems...> >::result
                    >::result;
  };
/******************************************************************************/
/*!
  \struct RemoveFirst
  \brief
    Specialization of RemoveFirst for the general case where Num and Head
    are the same. The recursion stops and the rest of the list is returned.
*/
/******************************************************************************/
  template <T Num, T... elems>
  struct RemoveFirst<Num, List<Num, elems...> >
  {
    using result = List<elems...>;
  };
/******************************************************************************/
/*!
  \struct RemoveFirst
  \brief
    Specialization of RemoveFirst for the case where the item cannot be found
    after going through the whole list (there is only one element left)
    The recursion stops and the last element is returned.
*/
/******************************************************************************/
  template <T Num, T Head>
  struct RemoveFirst<Num, List<Head>>
  {
    using result = List<Head>;
  };
/******************************************************************************/
/*!
  \struct RemoveFirst
  \brief
    Specialization of RemoveFirst for the case where the item to be removed
    is the only item in the list. Returns a empty list.
*/
/******************************************************************************/
  template <T Num>
  struct RemoveFirst<Num, List<Num>>
  {
    using result = List<>;
  };
/******************************************************************************/
/*!
  \struct Min
  \brief
    Min is a metafunction that takes a list and returns the smallest item 
    on the list. The specific definition of "smaller" depends on the type 
    of Comp,which is defaulted to std::less<T>.
*/
/******************************************************************************/
  template <typename L>
  struct Min;
/******************************************************************************/
/*!
  \struct Min
  \brief
    Specialization of Min for the general case.
    Uses the ternary operator and creates a Comp object that compares
    Smallest and Head and recursively instantiates Min templates with the
    template parameters that forms the list changed.
*/
/******************************************************************************/
  template <T Smallest, T Head, T... elems>
  struct Min< List<Smallest, Head, elems...> >
  {
    //construct Comp before calling
    constexpr static T result = Comp{}(Smallest, Head) ? 
    //if Smallest < Head we recursively call the next Min with head removed.
      Min< List<Smallest, elems...> >::result :
    //else we make Head the new smallest
      Min< List<Head, elems...> >::result;
  };
/******************************************************************************/
/*!
  \struct Min
  \brief 
    Specialization of Min for the case when there is only one element
    left in the list. Stops the recursion and returns the result.
*/
/******************************************************************************/
  template <T Smallest>
  struct Min< List<Smallest> >
  {
    //stop the recursion
    constexpr static T result = Smallest;
  };
/******************************************************************************/
/*!
  \struct Sort
  \brief 
    Sort is a metafunction that takes a list and returns the sorted ascending 
    order of the list.The definition of "smaller" depends on the type of Comp.
    It uses the selection sort algorithm to implement the sorting
    This is the base class template for Sort.
*/
/******************************************************************************/
  template <typename L>
  struct Sort;
/******************************************************************************/
/*!
  \struct Sort
  \brief
    Specialization of Sort for the general case when there are
    still elements in the unsorted part of the list.
    The sorted part of the list is recursively concatenated using Concat.
*/
/******************************************************************************/
  template <T...elems>
  struct Sort< List<elems...> >
  {
    //curr_min is the current minimum in the unsortedList
    constexpr static T curr_min = Min< List<elems...> >::result;
    //this is list that is not sorted yet (without the curr_min)
    using unsortedList = 
      typename RemoveFirst<curr_min, List<elems...> >::result;
    //recursively call Sort on the unsorted part of the list
    using recursiveSortList = typename Sort<unsortedList>::result;
    //Recursively concat the minimum elements with the unsorted list
    //which returns the sorted list.
    using result = typename Concat< List<curr_min>, recursiveSortList>::result;
  };
/******************************************************************************/
/*!
  \struct Sort
  \brief
    Specialization for Sort for the case when there is only one element
    left in the list. Stops the recursion and returns the last element
    (This is the largest element).
*/
/******************************************************************************/
  template <T elem>
  struct Sort< List<elem> >
  {
    //result is equal to the last unsorted element
    using result = List<elem>;
  };
/******************************************************************************/
/*!
  \struct Partition
  \brief 
    Partition is a metafunction that splits a list into 2 halves using the first
    element as a pivot. The first half consists of items strictly smaller
    than the pivot while the second half consists of items 
    strictly larger or equal to the pivot.
    This is the base class template for Partition.
*/
/******************************************************************************/
  template <typename L>
  struct Partition;
/******************************************************************************/
/*!
  \struct Partition_Impl
  \brief 
    Base class template for Partition_Impl that allow us to capture 
    3 parameter packs
    L1 -> contains elements smaller than pivot
    L2 -> contains elements larger or equal to pivot
    L3 -> contains elements that have not been checked yet
*/
/******************************************************************************/
  template <typename L1, typename L2, typename L3>
  struct Partition_Impl;
/******************************************************************************/
/*!
  \struct DummyPartition
  \brief
    This is a Dummy object that will contain the 2 parts of the list when
    the recursion in Partition_Impl stops.
*/
/******************************************************************************/
  template <typename L1, typename L2>
  struct DummyPartition
  {
    //first list
    using FirstPartition = L1;
    //second list
    using SecondPartition = L2;
  };
/******************************************************************************/
/*!
  \struct Partition
  \brief
     Specialization for the general case that instantiates the
     Partition_Impl templates. It also passes in two empty lists
     that will be used to store the 2 parts of the list when the function
     completes.
*/
/******************************************************************************/
  template <T... elems>
  struct Partition< List<elems...> >
  {
    //make 2 empty list that will be used in Partition_Impl
    using Implementation = typename Partition_Impl< List<>, List<>, 
                                                    List<elems...> 
                                                    >::result;
    //Implementation is a DummyPartition that contains 2 lists.
    using FirstHalf = typename Implementation::FirstPartition;
    using SecondHalf = typename Implementation::SecondPartition;
  };
/******************************************************************************/
/*!
  \struct Partition_Impl
  \brief
    Specialization of Partition_Impl for the general case. Recursively
    puts the Head of the third list into the first list if its smaller than 
    pivot and into the second list if its larger than pivot.
*/
/******************************************************************************/
  template <T Pivot, T Head, T... elems1, T... elems2, T... remainder>
  struct Partition_Impl< List<elems1...>, List<elems2...>, 
                         List<Pivot, Head, remainder...> 
                         >
  {
    //if head is smaller than pivot, put head into list 1
    using SmallerThanPivot = Partition_Impl< List<elems1..., Head>, 
                                             List<elems2...>, 
                                             List<Pivot, remainder...> 
                                             >;  
    //else put head into list2.
    using LargerThanPivot = Partition_Impl< List<elems1...>, 
                                            List<elems2..., Head>, 
                                            List<Pivot, remainder...> 
                                            >;
    //use std::conditional to decide which partition to recurse into.
    using result = typename std::conditional_t<Comp{}(Head, Pivot),
                                              SmallerThanPivot,
                                              LargerThanPivot
                                              >::result;                        
  };
/******************************************************************************/
/*!
  \struct Partition_Impl
  \brief
    Specialization for Partition_Impl for the case where there is only the
    pivot left in the 3rd list.
    Stops the Recursion and creates a DummyPartition and returns it as
    the result.
*/
/******************************************************************************/
  template <T Pivot, T... elems1, T... elems2>
  struct Partition_Impl< List<elems1...>, List<elems2...>, List<Pivot> >
  {
    //Create a DummyPartition that contains the two lists and stop recursion.
    using result = DummyPartition< List<elems1...>, List<elems2...>>;
  };
/******************************************************************************/
/*!
  \struct QuickSort
  \brief
    QuickSort is a metafunction that sorts a list using Partition.
    This is the base class template of QuickSort
    L -> contains the list to be sorted
    Dummy -> Default template parameter that is a dummy as C++ does
    not allow full template specializations of class templates
*/
/******************************************************************************/
  template <typename L, typename Dummy = void>
  struct QuickSort;
/******************************************************************************/
/*!
  \struct QuickSort
  \brief 
    Implements the following algorithm
      if list is empty, return
      Partition list into two halves around pivot value
      List1 -> Recursively QuickSort the first half
      List2 -> Recursively QuickSort the second half
      The sorted list is: List1 + pivot + List2
*/
/******************************************************************************/
  template <T Head, T... elems>
  struct QuickSort< List<Head, elems...> >
  {
    using FirstPart = typename Partition<List<Head, elems...> >::FirstHalf;
    using SecondPart = typename Partition<List<Head, elems...> >::SecondHalf;
    using List_1 = typename QuickSort<FirstPart>::result;
    using List_2 = typename QuickSort<SecondPart>::result;
    using List1_and_Pivot = typename Concat<List_1, List<Head>>::result;
    using result = typename Concat<List1_and_Pivot, List_2>::result;
  };
/******************************************************************************/
/*!
  \struct QuickSort
  \brief 
    Specialization of QuickSort for the case where the List is empty
    Stops the recursion and returns a empty list.
*/
/******************************************************************************/
  template <typename Dummy>
  struct QuickSort< List<>, Dummy>
  {
    using result = List<>;
  };
};//end Facility<T>

#endif //end FUNCTIONAL_LIST_H


