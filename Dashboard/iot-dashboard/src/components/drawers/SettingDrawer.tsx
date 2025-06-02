import {
  Drawer,
  DrawerClose,
  DrawerContent,
  DrawerDescription,
  DrawerFooter,
  DrawerHeader,
  DrawerTitle,
  DrawerTrigger
} from '../ui/drawer'
import { Button } from '../ui/button'
import { Settings } from 'lucide-react'

export function SettingDrawer() {
  return (
    <Drawer>
      <DrawerTrigger asChild>
        <Button variant="ghost">
          <Settings className="h-4 w-4" />
        </Button>
      </DrawerTrigger>
      <DrawerContent>
        <DrawerHeader className="text-left">
          <DrawerTitle>Setting</DrawerTitle>
          <DrawerDescription>
            Here is your telemetry settings to connect to
            ThingsBoard:
          </DrawerDescription>
        </DrawerHeader>
        {/* <SettingForm /> */}
        <DrawerFooter className="pt-2">
          <DrawerClose asChild>
            <Button variant="destructive">Cancel</Button>
          </DrawerClose>
        </DrawerFooter>
      </DrawerContent>
    </Drawer>
  )
}
