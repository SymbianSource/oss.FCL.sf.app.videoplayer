/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    XML Parser handler*
*/




#ifndef CIPTV3XMLCONTENTHANDLER_H
#define CIPTV3XMLCONTENTHANDLER_H

#include <e32std.h>
#include <xml/contenthandler.h>

class MIptvEpgVodCallback;
class CIptvEpgCategory;
class CIptvEpgContent;
class CIptvRssPlugin;
class CIptvMediaContent;
class CIptvTerminalVideoSupport;

// This is for MContentHandler XML parser.
using namespace Xml;

/**
 *  XML Parser handler
 *
 *  Implements parser callbacks and support functions.
 *  Stores interpreted XML content using given callback (SetVodCallback).
 *
 *  @lib IptvRssPlugin.dll
 *  @since S60 v3.1 rel3
 */
class CIptvXmlContentHandler : public CBase,
                               public MContentHandler
    {

public:

    /**
     * RSS Parser states.
     */
    enum TParserState
        {
        EIptvRssIdleState,
        EIptvRssState,
        EIptvRssChannelState,
        EIptvRssChannelDescriptionState,
        EIptvRssChannelLinkState,
        EIptvRssChannelTitleState,
        EIptvRssChannelImageState,
        EIptvRssChannelImageUrlState,
        EIptvRssItemState,
        EIptvRssItemDescriptionState,
        EIptvRssItemLinkState,
        EIptvRssItemTitleState,
        EIptvRssItemGuidState,
        EIptvRssItemEnclosureState,
        EIptvRssItemPubDate,
        EIptvRssItemMediaGroupState,
        EIptvRssItemMediaContentState,
        EIptvRssItemMediaThumbnailState,
        EIptvRssItemAuthorState,
        EIptvRssItemMediaCopyrightState,
        EIptvRssItemRatingState,
        EIptvRssItemMediaCategoryState,
        EIptvRssItemMediaCategoryEnclosureState
        };

    /**
     * Parent rating interpretation scheme.
     */
    enum TMediaRatingScheme
        {
        EMediaRatingNotSet,
        EMediaRatingSimple,
        EMediaRatingNum,
        EMediaRatingIcra,
        EMediaRatingMpaa,
        EMediaRatingVChip
        };

    /**
     * Control interpretation of categories and channels.
     */
    enum TCategoryState
        {
        ENoChannel,             // Initial value
        EChannelNoItem,         // Inside first channel, no items found yet
        EItemsWithCategory,     // Inside first channel, all items found
                                //   contains a category
        EItemsNoCategory,       // Inside first channel, all items found do
                                //   not contain a category
        EItemsBoth,             // Inside channel, both itmes with and
                                //   without category found
        ESkipFurtherChannels,   // A channel with categories processed.
                                //   No more channels will be accepted.
        ESkipFurtherCategories  // A channel without categories processed.
                                //   No categories inside further channels
                                //   will be accepted.
        };

    /**
     * Category state transition events.
     */
    enum TCategoryStateEvent
        {
        EChannelFound,
        EChannelCompleted,
        EItemFound,
        ECategoryFound,
        EItemCompleted
        };

    static CIptvXmlContentHandler* NewL(CIptvRssPlugin& aPlugin);

    ~CIptvXmlContentHandler();

    /**
     * Set callback for storing XML parse results.
     *
     * @param aVodCallback Pointer to callback interface.
     */
    void SetVodCallback( MIptvEpgVodCallback* aVodCallback );

// From base class MContentHandler

    /**
     * Callback to indicate the start of the document.
     *
     * @param aDocParam     Specifies the various parameters of the document.
     * @param aErrorCode    Parser error code.
     */
    void OnStartDocumentL(
        const RDocumentParameters& aDocParam,
        TInt aErrorCode );

    /**
     * Callback to indicate the end of the document.
     *
     * @param aErrorCode    Parser error code.
     */
    void OnEndDocumentL( TInt aErrorCode );

    /**
     * Callback to indicate an element parsing start.
     *
     * @param aElement      Handle to the element's details.
     * @param aAttributes   Contains the attributes for the element.
     * @param aErrorCode    Parser error code.
     */
    void OnStartElementL( const RTagInfo& aElement,
                          const RAttributeArray& aAttributes,
                          TInt aErrorCode );

    /**
     * Callback to indicate the element parsing completion.
     *
     * @param aElement      Handle to the element's details.
     * @param aErrorCode    Parser error code.
     */
    void OnEndElementL( const RTagInfo& aElement, TInt aErrorCode );

    /**
     * Callback that sends the content of the element.
     * Not all the content may be returned in one go. The data may be sent in
     * chunks. When an OnEndElementL is received this means there is no more
     * content to be sent.
     *
     * @param aBytes        Raw content data for the element.
     *                      The client is responsible for converting the data
     *                      to the required character set if necessary.
     *                      In some instances the content may be binary and
     *                      must not be converted.
     * @param aErrorCode    Parser error code.
     */
    void OnContentL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * Notification of the beginning of the scope of a prefix-URI Namespace
     * mapping.
     * This method is always called before the corresponding OnStartElementL
     * method.
     *
     * @param aPrefix       Namespace prefix being declared.
     * @param aUri          Namespace URI the prefix is mapped to.
     * @param aErrorCode    Parser error code.
     */
    void OnStartPrefixMappingL( const RString& aPrefix,
                                const RString& aUri,
                                TInt aErrorCode );

    /**
     * Notification of the end of the scope of a prefix-URI mapping.
     * This method is called after the corresponding DoEndElementL method.
     *
     * @param aPrefix       Namespace prefix that was mapped.
     * @param aErrorCode    Parser error code.
     */
    void OnEndPrefixMappingL( const RString& aPrefix, TInt aErrorCode );

    /**
     * Notification of ignorable whitespace in element content.
     *
     * @param aBytes        Ignored bytes from the document being parsed.
     * @param aErrorCode    Parser error code.
     */
    void OnIgnorableWhiteSpaceL( const TDesC8& aBytes, TInt aErrorCode );

    /**
     * Notification of a skipped entity. If the parser encounters an
     * external entity it does not need to expand it - it can return the
     * entity as aName for the client to deal with.
     *
     * @param aName         Name of the skipped entity.
     * @param aErrorCode    Parser error code.
     */
    void OnSkippedEntityL( const RString& aName, TInt aErrorCode );

    /**
     * Notification of a processing instruction.
     *
     * @param aTarget       Processing instruction target.
     * @param aData         Processing instruction data. If empty none was
     *                      supplied.
     * @param aErrorCode    Parser error code.
     */
    void OnProcessingInstructionL( const TDesC8& aTarget,
                                   const TDesC8& aData,
                                   TInt aErrorCode );

    /**
     * Notification of XML file parsing is failure.
     *
     * @param aErrorCode    Parser error code.
     */
    void OnError( TInt aErrorCode );

    /**
     * Obtains the interface matching the specified uid.
     *
     * @param aUid          Uid identifying the required interface.
     * @return              0 if no interface matching the uid is found.
     *                      Otherwise, the this pointer cast to that
     *                      interface.
     */
    TAny* GetExtendedInterface( const TInt32 aUid );

private:

    CIptvXmlContentHandler( CIptvRssPlugin& aPlugin );

    void ConstructL();

    /**
     * Handles item copyright value.
     */
    void CompleteCopyright();

    /**
     * Handles parent rating value.
     */
    void CompleteRating();

    /**
     * Handles item category value.
     */
    void CompleteCategoryL();

    /**
     * Handles item category value.
     * @param aName Name canditate for this category.
     */
    void CompleteCategoryL( const HBufC* aName );

    /**
     * Handles item author value.
     */
    void CompleteAuthor();

    /**
     * Handles item and category description value.
     */
    void CompleteDescription();

    /**
     * Handles item and category title value.
     */
    void CompleteTitle();

    /**
     * Handles item url link value.
     */
    void CompleteLink();

    /**
     * Handles item publish time value.
     */
    void CompletePubDate();

    /**
     * Handles item guid value.
     */
    void CompleteGuid();

    /**
     * Handles element attributes.
     *
     * @param aAttributeList  Parses element attributes..
     */
    void HandleEnclosureL(
        const RAttributeArray& aAttributeList );

    /**
     * Create a new category descriptor.
     *
     * @return              New empty category descriptor.
     */
    CIptvEpgCategory* CreateCategoryL() const;

    /**
     * Set a category for current item.
     * When category has been used for some other item it already exists and
     * is restored, Otherwise a new one is created and a key value given.
     *
     * @param aName         Name canditate to new category.
     */
    void SetItemCategoryL( const HBufC* aName );

    /**
     * Set channel as default category for current Item.
     * When channel category has already been used, restore it,
     * Otherwise save it and give a key value.
     */
    void SetChannelCategoryL();

    /**
     * Stores all categories used by current itam to database.
     */
    void AddCategories();

    /**
     * Add current content to dabase and link it to categories.
     */
    void AddContentL();

    /**
     * Create a new itme content descriptor.
     */
    void CreateContentL();

    /**
     * Adjust download type according to Mime type.
     *
     * @param aContent      Item descriptor.
     */
    void SetDownloadTypeFromMimeType(
        CIptvMediaContent& aContent ) const;

    /**
     * Item containing thumbnails is added to download queue.
     *
     * @param contentKey    Item identifier in database.
     * @param aThumbnailUrl Url to download thumbnail images.
     */
    void AddContentToDlQueueL(
        TUint32 contentKey,
        const TDesC& aThumbnailUrl );

    /**
     * Service thumbnail is added to download queue.
     *
     * @param aThumbnailUrl Url to download thumbnail images.
     */
    void AddServiceToDlQueueL(
        const TDesC& aThumbnailUrl );

    /**
     * Priorize item by type.
     */
    void PriorizeL();

    /**
     * Advances category state according to given event and decides whether
     * current object should be processed.
     *
     * @param aEvent        State transition event.
     * @return              Process or skip current object.
     */
    TBool SetCategoryState( TCategoryStateEvent aEvent );

private: // Data members

    /**
     * Calling class.
     */
    CIptvRssPlugin& iPlugin;

    /**
     * Callback for storing XML parse results.
     * Not own.
     */
    MIptvEpgVodCallback* iVodCallback;

    /**
     * Descriptor of current channel.
     * Own.
     */
    CIptvEpgCategory* iChannelCategory;

    /**
     * Category index key of current channel descriptor.
     * Value 0 means that channel has not been associated to any item
     * as category.
     */
    TUint32 iChannelCategoryKey;

    /**
     * Category index keys for current content.
     */
    RArray<TUint32> iCurrentCategoryKeys;

    /**
     * Descriptor of current item content.
     * Own.
     */
    CIptvEpgContent* iContent;

    /**
     * Xml parsing state.
     */
    TParserState iState;

    /**
     * Current xml content.
     * Own.
     */
    HBufC* iBuffer;

    /**
     * Indicates that we have accepted a channel thumbnail image as
     * service image to be downloaded.
     */
    TBool iServiceThumbnailAdded;

    /**
     * Indicates that we have found rss content from current feed.
     */
    TBool iRSSContentFound;

    /**
     * Total number of categories handled from xml.
     */
    TInt iCategoryCount;

    /**
     * Total number of content items handled from xml.
     */
    TInt iContentCount;

    /**
     * Media group descriptor found and we are prosessing group.
     */
    TBool iMediaGroup;

    /**
     * Current element name has media prefix.
     */
    TBool iMediaPrefix;

    /**
     * Description has been set with Media group or prefix value.
     * This shall not be overwritten by other values.
     */
    TBool iMediaDescriptionFixed;

    /**
     * Title has been set with Media group or prefix value.
     * This shall not be overwritten by other values.
     */
    TBool iMediaTitleFixed;

    /**
     * Rating has been set with Media group or prefix value.
     * This shall not be overwritten by other values.
     */
    TBool iMediaRatingFixed;

    /**
     * Copyrigth has been set with Media group or prefix value.
     * This shall not be overwritten by other values.
     */
    TBool iMediaCopyrightFixed;

    /**
     * Rating has been set with Media group or prefix value.
     * This shall not be overwritten by other values.
     */
    TBool iMediaThumbnailFixed;

    /**
     * Yahoo service is used.
     */
    TBool iYahoo;

    /**
     * Attribute set of current item.
     * Own.
     */
    RPointerArray<CIptvMediaContent> iMediaContents;

    /**
     * Priorized content set.
     * Not own.
     */
    RPointerArray<CIptvMediaContent> iMediaContentSet;

    /**
     * CIptvTerminalVideoSupport.
     * Own
     */
    CIptvTerminalVideoSupport* iTerminalVideoSupport;

    /**
     * Scheme to interpret current parent rating descriptor.
     */
    TMediaRatingScheme iMediaRatingScheme;

    /**
     * State to control handling of channel and item specific categories.
     */
    TCategoryState iCategoryState;

    /**
     * There is a specific category descriptor in current item.
     */
    TBool iCategoryFound;

    /**
     * Table to hold all valid categories.
     * Own.
     */
    RPointerArray<CIptvEpgCategory> iCategoryTable;

    };

#endif // CIPTV3XMLCONTENTHANDLER_H
