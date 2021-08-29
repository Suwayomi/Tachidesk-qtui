import QtQuick 2.8
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.0

import Tachidesk.Models 1.0

Item {
  id: libraryBase
  LibraryModel {
    id: libraryModel
    nm: networkManager
  }

  LibraryBase {
    anchors.fill: parent
  }
}
