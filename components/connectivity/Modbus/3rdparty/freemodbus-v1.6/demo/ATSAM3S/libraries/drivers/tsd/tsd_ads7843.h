#ifndef _TSD_ADS7843_
#define _TSD_ADS7843_

/*----------------------------------------------------------------------------
 *        External functions
 *----------------------------------------------------------------------------*/
extern void TSD_PenPressed( uint32_t dwX, uint32_t dwY ) ;
extern void TSD_PenMoved( uint32_t dwX, uint32_t dwY ) ;
extern void TSD_PenReleased( uint32_t dwX, uint32_t dwY ) ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void TSD_WaitPenPressed( void ) ;
extern void TSD_WaitPenReleased( void ) ;
extern void TSD_GetRawMeasurement( uint32_t *pData ) ;
extern void TSD_Initialize( int8_t calEn ) ;


#endif // _TSD_ADS7843_