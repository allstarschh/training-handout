/*************************************************************************
 *
 * Copyright (c) 2013-2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <KDE/Phonon/Global>

int main()
{
  // check if Phonon is available, by using one of the
  // constants defined in phononnamespace.h
  return Phonon::MusicCategory;
}
