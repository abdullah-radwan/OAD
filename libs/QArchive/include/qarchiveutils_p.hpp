#ifndef QARCHIVE_UTILS_PRIVATE_HPP_INCLUDED
#define QARCHIVE_UTILS_PRIVATE_HPP_INCLUDED

#ifndef OperationCanceled
#define OperationCanceled -1
#endif

#ifndef OperationPaused
#define OperationPaused -2
#endif

struct archive;
struct archive_entry;
class QString;

/* Smart pointer destructors. */
void ArchiveReadDestructor(archive*);
void ArchiveWriteDestructor(archive*);
void ArchiveEntryDestructor(archive_entry*);

/* Basic string manupilators.  */
char *concat(const char*, const char*);
QString getDirectoryFileName(const QString&);
#endif // QARCHIVE_UTILS_PRIVATE_HPP_INCLUDED
