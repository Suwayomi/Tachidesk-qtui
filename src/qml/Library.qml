import QtQuick 2.8
import QtQuick.Layouts 1.15
import QtGraphicalEffects 1.0

import Tachidesk.Models 1.0
import SortFilterProxyModel 0.2

Item {
  id: libraryBase
  SortFilterProxyModel {
    id: libraryModel
    sourceModel: library
    sorters: [
      RoleSorter {
        roleName: "title"
        sortOrder: Qt.AscendingOrder
      }
    ]
  }

  LibraryModel {
    id: library
    nm: networkManager
  }


  LibraryBase {
    anchors.fill: parent
  }
}
