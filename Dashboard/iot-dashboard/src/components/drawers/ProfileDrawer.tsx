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
import { ProfileForm } from '../forms/ProfileForm'
import { User } from 'lucide-react'
import { Dispatch, SetStateAction } from 'react'

export function ProfileDrawer({ isOpen, setIsOpen }: { isOpen: boolean, setIsOpen: Dispatch<SetStateAction<boolean>> }) {
  return (
    <Drawer open={isOpen} onOpenChange={(open) => {
      setIsOpen(open)
    }}>
      {/* <DrawerTrigger asChild>
        <Button variant="ghost">
          <User className="h-4 w-4" />
        </Button>
      </DrawerTrigger> */}
      <DrawerContent>
        <DrawerHeader className="text-left">
          <DrawerTitle>Profile</DrawerTitle>
          <DrawerDescription>
            View your profile information below.
          </DrawerDescription>
        </DrawerHeader>
        <ProfileForm />
        <DrawerFooter className="pt-2">
          <DrawerClose asChild>
            <Button variant="default">Close</Button>
          </DrawerClose>
        </DrawerFooter>
      </DrawerContent>
    </Drawer>
  )
}
