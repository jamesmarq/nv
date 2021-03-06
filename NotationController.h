//
//  NotationController.h
//  Notation
//
//  Created by Zachary Schneirov on 12/19/05.

/*Copyright (c) 2010, Zachary Schneirov. All rights reserved.
  Redistribution and use in source and binary forms, with or without modification, are permitted 
  provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright notice, this list of conditions 
     and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice, this list of 
	 conditions and the following disclaimer in the documentation and/or other materials provided with
     the distribution.
   - Neither the name of Notational Velocity nor the names of its contributors may be used to endorse 
     or promote products derived from this software without specific prior written permission. */


#import <Cocoa/Cocoa.h>
#import "FastListDataSource.h"
#import "LabelsListController.h"
#import "WALController.h"

//enum { kUISearch, kUINewNote, kUIDeleteNote, kUIRenameNote, kUILabelOperation };

typedef struct _NoteCatalogEntry {
    UTCDateTime lastModified;
    UInt32 logicalSize;
    OSType fileType;
    UInt32 nodeID;
    CFMutableStringRef filename;
    UniChar *filenameChars;
    UniCharCount filenameCharCount;
} NoteCatalogEntry;

@class NoteObject;
@class DeletedNoteObject;
@class SyncSessionController;
@class NotationPrefs;
@class NoteAttributeColumn;
@class NoteBookmark;
@class GlobalPrefs;

@interface NotationController : NSObject {
    NSMutableArray *allNotes;
    FastListDataSource *notesListDataSource;
    LabelsListController *labelsListController;
	GlobalPrefs *prefsController;
	SyncSessionController *syncSessionController;
	id delegate;
	
	float titleColumnWidth;
	NoteAttributeColumn* sortColumn;
	
    NoteObject **allNotesBuffer;
	unsigned int allNotesBufferSize;
    
    NSUInteger selectedNoteIndex;
    char *currentFilterStr, *manglingString;
    int lastWordInFilterStr;
    
	BOOL directoryChangesFound;
    
    NotationPrefs *notationPrefs;
	
	NSMutableSet *deletedNotes;
    
	int volumeSupportsExchangeObjects;
    FNSubscriptionUPP subscriptionCallback;
    FNSubscriptionRef noteDirSubscription;
    FSCatalogInfo *fsCatInfoArray;
    HFSUniStr255 *HFSUniNameArray;
	    
    size_t catEntriesCount, totalCatEntriesCount;
    NoteCatalogEntry *catalogEntries, **sortedCatalogEntries;
    
	unsigned int lastCheckedDateInHours;
    long blockSize;
    FSRef noteDirectoryRef, noteDatabaseRef;
    AliasHandle aliasHandle;
    BOOL aliasNeedsUpdating;
    OSStatus lastWriteError;
    
    WALStorageController *walWriter;
    NSMutableSet *unwrittenNotes;
	BOOL notesChanged;
	NSTimer *changeWritingTimer;
	NSUndoManager *undoManager;
}

- (id)init;
- (id)initWithAliasData:(NSData*)data error:(OSStatus*)err;
- (id)initWithDefaultDirectoryReturningError:(OSStatus*)err;
- (id)initWithDirectoryRef:(FSRef*)directoryRef error:(OSStatus*)err;
- (void)setAliasNeedsUpdating:(BOOL)needsUpdate;
- (BOOL)aliasNeedsUpdating;
- (NSData*)aliasDataForNoteDirectory;
- (OSStatus)_readAndInitializeSerializedNotes;
- (void)processRecoveredNotes:(NSDictionary*)dict;
- (BOOL)initializeJournaling;
- (void)handleJournalError;
- (void)checkJournalExistence;
- (void)closeJournal;
- (BOOL)flushAllNoteChanges;
- (void)flushEverything;

- (void)upgradeDatabaseIfNecessary;

- (id)delegate;
- (void)setDelegate:(id)theDelegate;

- (void)databaseEncryptionSettingsChanged;
- (void)databaseSettingsChangedFromOldFormat:(int)oldFormat;

- (int)currentNoteStorageFormat;
- (void)synchronizeNoteChanges:(NSTimer*)timer;

- (void)updateDateStringsIfNecessary;
- (void)restyleAllNotes;
- (void)setUndoManager:(NSUndoManager*)anUndoManager;
- (NSUndoManager*)undoManager;
- (void)noteDidNotWrite:(NoteObject*)note errorCode:(OSStatus)error;
- (void)scheduleWriteForNote:(NoteObject*)note;
- (void)trashRemainingNoteFilesInDirectory;
- (void)checkIfNotationIsTrashed;
- (void)updateLinksToNote:(NoteObject*)aNoteObject fromOldName:(NSString*)oldname;
- (void)addNotes:(NSArray*)noteArray;
- (void)addNotesFromSync:(NSArray*)noteArray;
- (void)addNewNote:(NoteObject*)aNoteObject;
- (void)_addNote:(NoteObject*)aNoteObject;
- (void)removeNote:(NoteObject*)aNoteObject;
- (void)removeNotes:(NSArray*)noteArray;
- (void)_purgeAlreadyDistributedDeletedNotes;
- (void)removeSyncMDFromDeletedNotesInSet:(NSSet*)notesToOrphan forService:(NSString*)serviceName;
- (DeletedNoteObject*)_addDeletedNote:(id<SynchronizedNote>)aNote;
- (void)_registerDeletionUndoForNote:(NoteObject*)aNote;
- (NoteObject*)addNote:(NSAttributedString*)attributedContents withTitle:(NSString*)title;
- (NoteObject*)addNoteFromCatalogEntry:(NoteCatalogEntry*)catEntry;

- (void)note:(NoteObject*)note didAddLabelSet:(NSSet*)labelSet;
- (void)note:(NoteObject*)note didRemoveLabelSet:(NSSet*)labelSet;

- (void)filterNotesFromLabelAtIndex:(int)labelIndex;
- (void)filterNotesFromLabelIndexSet:(NSIndexSet*)indexSet;

- (void)refilterNotes;
- (BOOL)filterNotesFromString:(NSString*)string;
- (BOOL)filterNotesFromUTF8String:(const char*)searchString forceUncached:(BOOL)forceUncached;
- (NSUInteger)preferredSelectedNoteIndex;
- (BOOL)preferredSelectedNoteMatchesSearchString;
- (NoteObject*)noteObjectAtFilteredIndex:(int)noteIndex;
- (NSArray*)notesAtIndexes:(NSIndexSet*)indexSet;
- (NSIndexSet*)indexesOfNotes:(NSArray*)noteSet;
- (NSUInteger)indexInFilteredListForNoteIdenticalTo:(NoteObject*)note;
- (NSUInteger)totalNoteCount;

- (void)scheduleUpdateListForAttribute:(NSString*)attribute;
- (NoteAttributeColumn*)sortColumn;
- (void)setSortColumn:(NoteAttributeColumn*)col;
- (void)resortAllNotes;
- (void)sortAndRedisplayNotes;

- (float)titleColumnWidth;
- (void)regeneratePreviewsForColumn:(NSTableColumn*)col visibleFilteredRows:(NSRange)rows forceUpdate:(BOOL)force;
- (void)regenerateAllPreviews;

//for setting up the nstableviews
- (id)labelsListDataSource;
- (id)notesListDataSource;

- (SyncSessionController*)syncSessionController;

- (void)dealloc;

@end


enum { NVDefaultReveal = 0, NVDoNotChangeScrollPosition = 1, NVOrderFrontWindow = 2, NVEditNoteToReveal = 4 };

@interface NSObject (NotationControllerDelegate)
- (BOOL)notationListShouldChange:(NotationController*)someNotation;
- (void)notationListMightChange:(NotationController*)someNotation;
- (void)notationListDidChange:(NotationController*)someNotation;
- (void)notation:(NotationController*)notation revealNote:(NoteObject*)note options:(NSUInteger)opts;
- (void)notation:(NotationController*)notation revealNotes:(NSArray*)notes;

- (void)contentsUpdatedForNote:(NoteObject*)aNoteObject;
- (void)titleUpdatedForNote:(NoteObject*)aNoteObject;
- (void)rowShouldUpdate:(NSInteger)affectedRow;

@end
