#ifndef ORDER_BOOK_DEFINES_H
#define ORDER_BOOK_DEFINES_H
#include <boost/unordered_map.hpp>
#include <apiDataTypes.h>

namespace CMD {

  /**
 *
 */
  class SingleOrder;
}
namespace API2   {

  /**
 * @brief The OrderBook struct
 */
  struct OrderBook
  {
    /**
     * @brief insert
     * @param order
     */
    void insert(CMD::SingleOrder* order);

    /**
     * @brief insert
     * @param order
     */
    void insert(SingleOrder* order);


    /**
     * @brief getOrder
     * @param clOrderId
     * @return
     */
    CMD::SingleOrder* getOrder(DATA_TYPES::CLORDER_ID clOrderId);


    /**
     * @brief getApiOrder
     * @param clOrderId
     * @return
     */
    SingleOrder* getApiOrder(DATA_TYPES::CLORDER_ID clOrderId);

    /**
     * @brief update
     * @param confirmation
     * @param reconcileOldOrders
     * @return
     */
    int update(OrderConfirmation& confirmation, bool reconcileOldOrders = false);

    /**
     * @brief alignOrders
     * @param order
     */
    void alignOrders(CMD::SingleOrder * order);
    
    /**
     * @brief isCancelOrderPending checks if an cancel order is in pending state or not
     * param cancelOrderId
     * return true if order is in pending state else return false
     */
    bool isCancelOrderPending( const UNSIGNED_LONG cancelOrderId );

    // Client order Id to order mapping
    boost::unordered_map<DATA_TYPES::CLORDER_ID, CMD::SingleOrder*> _orderBook;
    //boost::unordered_map<long, CMD::ThreeLegOrder*> _orderBookThreeLeg;

  };

}
#endif
