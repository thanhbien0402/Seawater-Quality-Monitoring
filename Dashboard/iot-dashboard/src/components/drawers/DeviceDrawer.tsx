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
import { SquarePlus } from 'lucide-react'
import { useState } from 'react'
import { DeviceForm } from '../forms/DeviceForm'

export function DeviceDrawer() {
    const [isOpen, setIsOpen] = useState<boolean>(false)

    return (
        <Drawer open={isOpen} onOpenChange={(open) => {
            setIsOpen(open)
        }}>
            <DrawerTrigger asChild>
                <Button variant="ghost">
                    <p>Add new device</p>
                    <SquarePlus className="h-4 w-4" />
                </Button>
            </DrawerTrigger>
            <DrawerContent>
                <DrawerHeader className="text-left">
                    <DrawerTitle>Profile</DrawerTitle>
                    <DrawerDescription>
                        View your profile information below.
                    </DrawerDescription>
                </DrawerHeader>
                <DeviceForm setIsOpen={setIsOpen} />
            </DrawerContent>
        </Drawer>
    )
}
